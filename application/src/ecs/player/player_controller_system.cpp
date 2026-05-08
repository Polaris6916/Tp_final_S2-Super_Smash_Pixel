/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "ecs/player/player_controller_system.h"
#include "ecs/player/player_utils.h"
#include "ecs/common/damage_utils.h"
#include "ecs/common/terrain_utils.h"

void PlayerControllerSystem::OnFixedUpdate(EntityCommandBuffer& ecb)
{
    float delta = m_scene->GetDelta();

    auto oneWayView = m_registry.view<const PlayerControllerInput, OneWayPass>();
    for (auto [entity, input, oneWayPass] : oneWayView.each())
    {
        if (input.oneWayPassDown) oneWayPass.delay = 0.2f;
    }

    auto view = m_registry.view<
        Rigidbody,
        Damageable,
        const PlayerControllerInput,
        PlayerController,
        const GroundContact,
        const PlayerAffiliation,
        PlayerAnimInfo
    >();
    for (auto [entity, rigidbody, damageable, input, controller, ground, affiliation, animInfo] : view.each())
    {
        b2Vec2 position = b2Body_GetPosition(rigidbody.bodyId);
        b2Vec2 velocity = b2Body_GetLinearVelocity(rigidbody.bodyId) - ground.groundVelocity;

        controller.isStateUpdated = false;
        controller.prevState = controller.currState;

        // Fall
        if (position.y < -5.f || fabsf(position.x) > 40.f || position.y > 25.f)
        {
            bool destroyPlayer = OnPlayerFall(entity, rigidbody, affiliation, damageable, controller);
            if (destroyPlayer)
            {
                ecb.DestroyEntity(entity);
            }
            continue;
        }

        // Ground
        if (ground.wasGrounded == false && ground.isGrounded)
        {
            controller.bonusJumpCount = controller.maxBonusJumpCount;
            controller.airAttackCount = 1;
            m_scene->GetAssetManager()->PlaySoundFX(SFX_LAND);

            // TODO 10 - Appeler la fonction PlayerUtils::EmitLandDust pour émettre de la poussière.
            //      - La scène est accessible via le membre m_scene
            //      - On peut savoir si le personnage regarde vers la droite avec le champ facingRight du controller.
            PlayerUtils::EmitLandDust(m_scene, position, controller.facingRight);
        }


        // Shield & Damage
        FixedUpdateShieldAndDamage(affiliation, ground, controller, damageable);

        // State
        FixedUpdateState(rigidbody, damageable, input, controller, ground, animInfo);

        if (controller.isStateUpdated)
        {
            FixedUpdateOnStateChanged(rigidbody, damageable, input, controller, ground);
        }

        // Physics
        FixedUpdateAutoVelocity(controller, rigidbody, ground, animInfo);
        FixedUpdatePhysics(entity, rigidbody, damageable, input, controller, ground, animInfo);

        // Finalise step
        DamageUtils::ResetLastDamager(damageable);
        DamageUtils::ResetCumulativeDamage(damageable);

        animInfo.type = PlayerAnimInfo::Event::NONE;

        damageable.lockTime -= delta;
        damageable.lockAttackTime -= delta;
        controller.delayAttack -= delta;
        controller.delayEarlyJump -= delta;

        // TODO 16 - Décommentez le code suivant qui met à jour des délais
        //        dont vous aurez éventuellement besoin pour la partie libre.

        controller.delayBonusJump -= delta;
        controller.delayCoyoteJump -= delta;
        controller.delayRoll -= delta;
        if (input.attackDown == true)
        {
            controller.delaySmashReleaseMin -= delta;
            controller.delaySmashReleaseMax -= delta;
        }
        if (ground.isGrounded) controller.delayClearLastDamager -= delta;

        if (controller.shieldPower < 1 && controller.currState != PlayerState::DEFEND) controller.shieldPower += 0.005f;
    }
}

void PlayerControllerSystem::OnUpdate(EntityCommandBuffer &ecb)
{
    auto view = m_registry.view<const PlayerControllerInput, PlayerController, Sprite>();
    for (auto [entity, input, controller, sprite] : view.each())
    {
        if (input.attackPressed)
        {
            controller.delayAttack = 0.2f;
        }
        if (input.jumpPressed)
        {
            // TODO 4 - Donnez la valeur 0.1f au champ delayEarlyJump du controller.
            controller.delayEarlyJump = 0.1f;
        }

        sprite.flip = controller.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    }
}

bool PlayerControllerSystem::OnPlayerFall(
    entt::entity entity,
    const Rigidbody &rigidbody,
    const PlayerAffiliation &affiliation,
    Damageable &damageable,
    PlayerController &controller)
{
    controller.delayAttack = -1.f;
    controller.delayBonusJump = -1.f;
    controller.delayClearLastDamager = -1.f;
    controller.delayCoyoteJump = -1.f;
    controller.delayEarlyJump = -1.f;
    controller.delayRoll = -1.f;
    controller.delaySmashReleaseMax = -1.f;
    controller.shieldPower = -1.f;

    DamageUtils::ResetDamageable(damageable);

    int fallCount = INT_MAX;
    int lastDamagerID = controller.lastDamagerAffiliation.playerID;
    PlayerStats *damagerStats = g_gameCommon.GetPlayerStats(lastDamagerID);
    if (damagerStats)
    {
        damagerStats->koCount++;
    }
    PlayerStats *playerStats = g_gameCommon.GetPlayerStats(affiliation.playerID);
    if (playerStats)
    {
        playerStats->ejectionScore = 0.f;
        playerStats->fallCount++;
        fallCount = playerStats->fallCount;
    }

    const StageConfig &stageConfig = g_gameCommon.stageConfig;
    if (stageConfig.mode == StageConfig::Mode::LIMITED_LIVES)
    {
        if (fallCount >= stageConfig.lifeCount)
        {
            return true;
        }
    }

    b2Transform xf{ b2Vec2{ 0.f, 8.f }, b2Rot_identity };
    if (m_registry.all_of<FixedUpdateTransform>(entity))
    {
        auto &fixedXf = m_registry.get<FixedUpdateTransform>(entity);
        fixedXf.Set(xf);
    }

    controller.hVelocity = 0.f;
    controller.delayAttack = -1.f;
    controller.delayEarlyJump = -1.f;

    damageable.ejectionScore = 0.f;
    damageable.lockTime = -1.f;
    damageable.lockAttackTime = -1.f;

    b2Body_SetTransform(rigidbody.bodyId, xf.p, xf.q);
    b2Body_SetLinearVelocity(rigidbody.bodyId, b2Vec2_zero);

    return false;
}

void PlayerControllerSystem::FixedUpdateShieldAndDamage(
    const PlayerAffiliation &affiliation, const GroundContact &ground, PlayerController &controller, Damageable &damageable)
{
    const float score = damageable.cumulativeDamage.amount;
    if (controller.currState == PlayerState::ROLL || score < 1.f)
    {
        return;
    }

    controller.lastDamagerEntity = damageable.lastDamagerEntity;
    controller.lastDamagerAffiliation = damageable.lastDamagerAffiliation;

    if (controller.currState == PlayerState::DEFEND)
    {

        controller.shieldPower -= score/30.f;
        return;
    };

    if (controller.currState == PlayerState::ROLL)
    {
        return;
    };
    if (controller.currState != PlayerState::LAUNCHED)
    {
        PlayerUtils::SetState(controller, PlayerState::TAKE_DAMAGE);
        controller.isStateUpdated = true;
    }
    
    damageable.ejectionScore += score;
    damageable.lockTime = fmaxf(
        damageable.lockTime,
        damageable.cumulativeDamage.lockTime);
    damageable.lockAttackTime = fmaxf(
        damageable.lockAttackTime,
        damageable.cumulativeDamage.lockAttackTime);

    // Stats
    int lastDamagerID = controller.lastDamagerAffiliation.playerID;
    PlayerStats *damagerStats = g_gameCommon.GetPlayerStats(lastDamagerID);
    PlayerStats *playerStats = g_gameCommon.GetPlayerStats(affiliation.playerID);
    if (damagerStats)
    {
        // BONUS - Modifier les stats du damager
    }
    if (playerStats)
    {
        playerStats->ejectionScore = damageable.ejectionScore;
        // BONUS - Modifier les stats du player
    }
}

void PlayerControllerSystem::FixedUpdateState(
    const Rigidbody &rigidbody,
    Damageable &damageable,
    const PlayerControllerInput &input,
    PlayerController &controller,
    const GroundContact &ground,
    PlayerAnimInfo &event)
{
    b2BodyId bodyId = rigidbody.bodyId;
    b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId) - ground.groundVelocity;

    switch (event.type)
    {
    case PlayerAnimInfo::Event::SMASH_END:
    case PlayerAnimInfo::Event::COMBO_END:
        PlayerUtils::SetState(controller, PlayerState::IDLE);
        break;
    case PlayerAnimInfo::Event::ROLL_END:
        PlayerUtils::SetState(controller, PlayerState::IDLE);
        break;
    default:
        break;
    }

    //----------------------------------------------------------------------
    // Conditions de sortie

    if (damageable.cumulativeDamage.ejectionType != Damage::Type::NO_EJECTION)
    {
        // Début d'une éjection
        PlayerUtils::SetState(controller, PlayerState::LAUNCHED);
        damageable.lockTime = -1.f;
        controller.isStateUpdated = true;
        return;
    }
    if (controller.currState == PlayerState::LAUNCHED)
    {
        // Suite d'une éjection
        if (velocity.y > -4.f && b2Length(velocity) > 1.f)
            return;
    }
    if (damageable.lockTime > 0.f)
    {
        // Personnage bloqué (parce qu'il vient de subir des dommages)
        return;
    }
    if (controller.currState == PlayerState::ROLL) {
        return;
    }
    if (controller.prevState == PlayerState::ROLL && controller.currState != PlayerState::ROLL)
    {
        controller.delayRoll = 1.f;
    }

    // Mouvement
    if (PlayerUtils::IsAttacking(controller) == false && input.defendDown && ground.isGrounded)
    {
        // Bouclier
        if (input.direction != 0.f && controller.delayRoll< 0.f)
        {
            PlayerUtils::SetState(controller, PlayerState::ROLL);
            return;
        }
        else if (controller.shieldPower>0)
        {
            PlayerUtils::SetState(controller, PlayerState::DEFEND);
            return;
        }
    }

    if (ground.isGrounded)
    {
        //----------------------------------------------------------------------
        // Etat au sol

        if (PlayerUtils::IsAttacking(controller))
        {
            if (controller.currState == PlayerState::SMASH_HOLD)
            {
                // TODO 17 - Déterminez si le personnage doit passer dans l'état
                //        PlayerState::SMASH_RELEASE
                //        Utilisez input.attackDown
                if ((input.attackDown == false && controller.delaySmashReleaseMin <=0) || controller.delaySmashReleaseMax <= 0)
                {
                    PlayerUtils::SetState(controller, PlayerState::SMASH_RELEASE);
                    controller.delaySmashReleaseMin = 1.f;
                }
                else if (input.attackDown == false && controller.delaySmashReleaseMin > 0)
                {
                    PlayerUtils::SetState(controller, PlayerState::IDLE);
                    controller.delaySmashReleaseMin = 0.5f;
                    controller.delaySmashReleaseMax = 3.f;
                }
                
                // BONUS - Utilisez aussi controller.delaySmashReleaseMin et
                //         controller.delaySmashReleaseMax pour le passage à l'état SMASH_RELEASE
            }
        }
        else
        {
            if (controller.delayAttack > 0.f && damageable.lockAttackTime <= 0.f)
            {
                // TODO 11 - Déterminez quelle attaque le personnage doit commencer.
                //        Utilisez input.attackType pour attribuer les états
                //        SMASH_HOLD, ATTACK_SPECIAL ou ATTACK_COMBO.
                if (input.attackType == AttackType::SMASH)
                {
                    PlayerUtils::SetState(controller, PlayerState::SMASH_HOLD);
                }
                else if (input.attackType == AttackType::SPECIAL)
                {
                    PlayerUtils::SetState(controller, PlayerState::ATTACK_SPECIAL);
                }
                else if (input.attackType == AttackType::COMBO)
                {
                    PlayerUtils::SetState(controller, PlayerState::ATTACK_COMBO);
                }
                controller.delayAttack = -1.f;
            }
            else
            {
                // TODO 9 - Déterminez si le personnage doit être dans l'état IDLE ou RUN.
                //        Testez si (fabsf(velocity.x) < 0.5f) pour savoir si le personnage
                //        a une vitesse horizontale suffisante.
                if (fabsf(velocity.x) < 0.5f)
                {
                    PlayerUtils::SetState(controller, PlayerState::IDLE);
                }
                else
                {
                    PlayerUtils::SetState(controller, PlayerState::RUN);
                }
            }
        }
    }
    else
    {
        //----------------------------------------------------------------------
        // Etat en l'air

        if (PlayerUtils::IsAttacking(controller) == false)
        {
            if (controller.currState != PlayerState::JUMP &&
                controller.currState != PlayerState::FALL)
            {
                PlayerUtils::SetState(controller, PlayerState::JUMP);
            }

            if (controller.currState == PlayerState::JUMP && velocity.y <= 2.f)
            {
                PlayerUtils::SetState(controller, PlayerState::FALL);
            }
            else if (controller.currState == PlayerState::FALL && velocity.y > 6.f)
            {
                PlayerUtils::SetState(controller, PlayerState::JUMP);
            }
        }
        
        // BONUS - Codez ici l'état PlayerState::ATTACK_AIR
        if (PlayerUtils::IsAttacking(controller) == false && controller.airAttackCount >= 1 && controller.delayAttack >=0)
        {
            if (input.attackType == AttackType::COMBO)
            {
                controller.delayAttack = -1.f;
                PlayerUtils::SetState(controller, PlayerState::ATTACK_AIR);
                controller.airAttackCount--;
            }
        }
    }
    //--------------------------------------------------------------------------
    // Orientation du personnage

    if ((PlayerUtils::IsAttacking(controller) == false) && damageable.lockTime <= 0.f)
    {
        if ((input.direction > 0.f && !controller.facingRight) ||
            (input.direction < 0.f && controller.facingRight))
        {
            controller.facingRight = !controller.facingRight;
        }
    }
}

void PlayerControllerSystem::FixedUpdateOnStateChanged(
    const Rigidbody &rigidbody,
    Damageable &damageable,
    const PlayerControllerInput &input, 
    PlayerController &controller,
    const GroundContact &ground)
{
    const b2Vec2 position = b2Body_GetPosition(rigidbody.bodyId);
    switch (controller.currState)
    {
    case PlayerState::ATTACK_COMBO:
    case PlayerState::ATTACK_SPECIAL:
    case PlayerState::ATTACK_AIR:
        controller.delayAttack = -1.f;
        break;

        controller.delayAttack = -1.f;
        controller.airAttackCount = 0;
        break;

    case PlayerState::SMASH_HOLD:
        controller.delaySmashReleaseMin = 0.5f;
        controller.delaySmashReleaseMax = 3.f;
        break;

    case PlayerState::SMASH_RELEASE:
        controller.smashMultiplier = 1.f;
        controller.delaySmashReleaseMax = -1.f;
        break;

    case PlayerState::DEFEND:
        break;

    case PlayerState::ROLL_END:
        controller.delayRoll = 5.f;
        PlayerUtils::SetState(controller, PlayerState::IDLE);
        break;

    default:
        break;
    }
}

void PlayerControllerSystem::FixedUpdateAutoVelocity(
    PlayerController &controller,
    const Rigidbody &rigidbody,
    const GroundContact &ground,
    PlayerAnimInfo &animInfo)
{
    switch (controller.currState)
    {
    case PlayerState::ATTACK_COMBO:
    case PlayerState::ATTACK_SPECIAL:
    case PlayerState::SMASH_HOLD:
    case PlayerState::SMASH_RELEASE:
    case PlayerState::ROLL:
        controller.hasAutoVelocity = true;
        break;
    case PlayerState::DEFEND:
    case PlayerState::TAKE_DAMAGE:
        controller.hasAutoVelocity = true;
        animInfo.autoVelocity = 0.f;
        break;
    default:
        controller.hasAutoVelocity = false;
        animInfo.autoVelocity = 0.f;
        break;
    }

    // Il est possible d'effectuer ici une détection des bords du terrain
    // pour ne pas tomber en effectuant une attaque ou une roulade
}

void PlayerControllerSystem::FixedUpdatePhysics(
    entt::entity entity,
    Rigidbody &rigidbody,
    const Damageable &damageable,
    const PlayerControllerInput &input,
    PlayerController &controller,
    const GroundContact &ground,
    PlayerAnimInfo &animInfo)
{
    b2BodyId bodyId = rigidbody.bodyId;
    b2Vec2 position = b2Body_GetPosition(bodyId);
    b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId) - ground.groundVelocity;
    float targetHVelocity = 0.f;

    b2Body_SetGravityScale(bodyId, 1.f);

    //--------------------------------------------------------------------------
    // Conditions de sorties

    if (damageable.cumulativeDamage.ejectionType != Damage::Type::NO_EJECTION)
    {
        b2Vec2 ejection = DamageUtils::GetEjection(damageable, position);
        b2Body_SetLinearVelocity(bodyId, ejection);
        PlayerUtils::EmitEjectionParticles(m_scene, position, ejection);
        return;
    }

    if (controller.currState == PlayerState::LAUNCHED)
    {
        b2Body_SetGravityScale(bodyId, 0.5f);
        controller.hVelocity = velocity.x;
        return;
    }

    if (damageable.lockTime > 0.f)
        return;

    //--------------------------------------------------------------------------
    // Début d'un saut    

    // TODO 6 - Modifiez les conditions du saut.
    //      - Il faut un delayEarlyJump positif et être sur le sol.

    if (controller.delayEarlyJump > 0 && ground.isGrounded == true)
    {
        // TODO 6 - Modifiez la vitesse verticale pour lui donner la valeur du champ jumpImpulse du controller.
        //      - Puis donnez la valeur -1 au délai pour emp
        velocity.y = controller.jumpImpulse;
        controller.delayEarlyJump = -1.f;
   
        // TODO - Décommentez les lignes suivantes pour jouer un son et émettre un effet de poussière.
        m_scene->GetAssetManager()->PlaySoundFX(SFX_JUMP_GROUND);
        PlayerUtils::EmitJumpDust(m_scene, position, controller.facingRight);
    }
    else if (controller.bonusJumpCount > 0 && ground.isGrounded == false && controller.delayBonusJump <=0 && controller.delayEarlyJump >= 0) {
        velocity.y = controller.jumpImpulse;
        controller.delayEarlyJump = -1.f;
        m_scene->GetAssetManager()->PlaySoundFX(SFX_JUMP_GROUND);
        controller.bonusJumpCount -= 1;  
    }

    //--------------------------------------------------------------------------
    // Saut long

    if (ground.isGrounded == false && velocity.y > 0.f)
    {
        // TODO 7 - Appliquez un facteur de gravité de 0.5 si le champ jumpDown vaut true et 1.0 sinon.
        float scale = 1.f;
        if (input.jumpDown)
        {
            float scale = 0.5f;
        }
        
        b2Body_SetGravityScale(bodyId, scale);
    }

    //--------------------------------------------------------------------------
    // Définit la vitesse horizontale

    // TODO 5 - Mise a jour de la vitesse
    //      - Décommentez la ligne modifiant hVelocity

    /*controller.hVelocity = 0.f;
    if (controller.currState != PlayerState::DEFEND)
    {
        controller.hVelocity = controller.maxSpeed * input.direction;
    }*/

    // Calcul de la vitesse horizontale cible
    targetHVelocity = 0.f;
    if (controller.currState != PlayerState::DEFEND)
        targetHVelocity = controller.maxSpeed * input.direction;

    // Choix de l'accélération selon l'état (sol/air)
    float acc = 0;
    if (ground.isGrounded)
        acc = controller.groundAcc;
    else
        acc = controller.airAcc;

    float delta = m_scene->GetDelta();

    // Si une direction est pressée, accélère vers la cible
    if (input.direction != 0.f) {
        float diff = targetHVelocity - controller.hVelocity;
        float maxChange = acc * delta;
        if (diff > maxChange) diff = maxChange;
        else if (diff < -maxChange) diff = -maxChange;
        controller.hVelocity += diff;
    }
    else {
        // Sinon, applique une décélération (friction) vers 0
        float maxDecel = acc * delta;
        if (controller.hVelocity > 0.f) {
            controller.hVelocity -= maxDecel;
            if (controller.hVelocity < 0.f) controller.hVelocity = 0.f;
        }
        else if (controller.hVelocity < 0.f) {
            controller.hVelocity += maxDecel;
            if (controller.hVelocity > 0.f) controller.hVelocity = 0.f;
        }
    }


    if (controller.hasAutoVelocity)
    {
        velocity.x = animInfo.autoVelocity;
    }
    else
    {
        if (ground.isGrounded || !(ground.inContact)) velocity.x = controller.hVelocity;
    }

    velocity += ground.groundVelocity;

    // TODO 5- Mise a jour de la vitesse
    //      - Décommentez la ligne affectant la vitesse calculée au corps
    b2Body_SetLinearVelocity(bodyId, velocity);
}
