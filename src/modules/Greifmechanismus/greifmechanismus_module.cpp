#include "greifmechanismus_module.h"

#include "actuators/motor_module_Arm.h"
#include "actuators/servo_module_Arm.h"

namespace {
arm_drehkranz::ServoModule* g_servo_drehkranz = nullptr;
arm_lenkung::ServoModule*   g_servo_lenkung   = nullptr;
MotorModuleArm*             g_motor_arm       = nullptr;

void initActuators()
{
    if (!g_servo_drehkranz) g_servo_drehkranz = new arm_drehkranz::ServoModule();
    if (!g_servo_lenkung)   g_servo_lenkung   = new arm_lenkung::ServoModule();
    if (!g_motor_arm)       g_motor_arm       = new MotorModuleArm();
}

void goToStoragePosition(int pos)
{
    if (pos == 1) {
        g_servo_drehkranz->setSteeringAngle(gripper_cfg::LAGER_POS_1_D);
        g_servo_lenkung->setSteeringAngle(gripper_cfg::LAGER_POS_1_L);
    } else if (pos == 2) {
        g_servo_drehkranz->setSteeringAngle(gripper_cfg::LAGER_POS_2_D);
        g_servo_lenkung->setSteeringAngle(gripper_cfg::LAGER_POS_2_L);
    } else if (pos == 3) {
        g_servo_drehkranz->setSteeringAngle(gripper_cfg::LAGER_POS_3_D);
        g_servo_lenkung->setSteeringAngle(gripper_cfg::LAGER_POS_3_L);
    } else if (pos == 4) {
        g_servo_drehkranz->setSteeringAngle(gripper_cfg::LAGER_POS_4_D);
        g_servo_lenkung->setSteeringAngle(gripper_cfg::LAGER_POS_4_L);
    }
}

void pickOrDropHouse(float angle_D, float angle_L, float seil_Umdrehungen)
{
    g_servo_drehkranz->setSteeringAngle(angle_D);
    g_servo_lenkung->setSteeringAngle(angle_L);
    g_motor_arm->setAndWait(seil_Umdrehungen);
    g_motor_arm->setAndWait(seil_Umdrehungen * -1);
}

void pickOrDropStorage(int pos, float seil_Umdrehungen)
{
    goToStoragePosition(pos);
    g_motor_arm->setAndWait(seil_Umdrehungen);
    g_motor_arm->setAndWait(seil_Umdrehungen * -1);
}

void storeColorInFirstFreeSlot(int farbe)
{
    if (lagern::g_lager_pos_1 == K_LAGER_LEER) {
        pickOrDropStorage(1,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_1);
        lagern::g_lager_pos_1 = farbe;
    } else if (lagern::g_lager_pos_2 == K_LAGER_LEER) {
        pickOrDropStorage(2,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_2);
        lagern::g_lager_pos_2 = farbe;
    } else if (lagern::g_lager_pos_3 == K_LAGER_LEER) {
        pickOrDropStorage(3,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_3);
        lagern::g_lager_pos_3 = farbe;
    } else if (lagern::g_lager_pos_4 == K_LAGER_LEER) {
        pickOrDropStorage(4,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_4);
        lagern::g_lager_pos_4 = farbe;
    }
}

void unloadColorFromSlotIfPresent(int farbe)
{
    if (lagern::g_lager_pos_1 == farbe) {
        pickOrDropStorage(1,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_1);
        lagern::g_lager_pos_1 = K_LAGER_LEER;
    } else if (lagern::g_lager_pos_2 == farbe) {
        pickOrDropStorage(2,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_2);
        lagern::g_lager_pos_2 = K_LAGER_LEER;
    } else if (lagern::g_lager_pos_3 == farbe) {
        pickOrDropStorage(3,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_3);
        lagern::g_lager_pos_3 = K_LAGER_LEER;
    } else if (lagern::g_lager_pos_4 == farbe) {
        pickOrDropStorage(4,gripper_cfg::SEIL_ROTATIONEN_LAGER_POS_4);
        lagern::g_lager_pos_4 = K_LAGER_LEER;
    }
}
} // namespace

namespace gripper_cfg
{
bool lager = false;
} // namespace gripper_cfg

namespace lagern
{
int g_lager_pos_1 = 0;
int g_lager_pos_2 = 0;
int g_lager_pos_3 = 0;
int g_lager_pos_4 = 0;
} // namespace lagern

namespace aufnehmen
{
AufnehmenModule::AufnehmenModule()
{
    initActuators();
    g_servo_drehkranz->initialize();
    g_servo_lenkung->initialize();
}

void AufnehmenModule::aufnehmenRot()
{
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_ROT_GELB_D,gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_L,gripper_cfg::SEIL_ROTATIONEN_HAUS_ROT_GELB);
    if (gripper_cfg::lager) {
        storeColorInFirstFreeSlot(K_FARBE_ROT);
    }
}

void AufnehmenModule::aufnehmenBlau()
{
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_D,gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_L,gripper_cfg::SEIL_ROTATIONEN_HAUS_BLAU_GRUEN);
    if (gripper_cfg::lager) {    
    storeColorInFirstFreeSlot(K_FARBE_BLAU);
    }
}

void AufnehmenModule::aufnehmenGelb()
{
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_ROT_GELB_D,gripper_cfg::AUFNEHMEN_ABLEGEN_POS_ROT_GELB_L,gripper_cfg::SEIL_ROTATIONEN_HAUS_ROT_GELB);
    if (gripper_cfg::lager) {
        storeColorInFirstFreeSlot(K_FARBE_GELB);
    }
}

void AufnehmenModule::aufnehmenGruen()
{
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_D,gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_L,gripper_cfg::SEIL_ROTATIONEN_HAUS_BLAU_GRUEN);
    if (gripper_cfg::lager) {
        storeColorInFirstFreeSlot(K_FARBE_GRUEN);
    }
}
} // namespace aufnehmen

namespace abladen
{
AbladenModule::AbladenModule()
{
    initActuators();
    g_servo_drehkranz->initialize();
    g_servo_lenkung->initialize();
}

void AbladenModule::abladenRot()
{
    if (gripper_cfg::lager) {
        unloadColorFromSlotIfPresent(K_FARBE_ROT);
    }
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_ROT_GELB_D, gripper_cfg::AUFNEHMEN_ABLEGEN_POS_ROT_GELB_L, gripper_cfg::SEIL_ROTATIONEN_HAUS_ROT_GELB);
}

void AbladenModule::abladenBlau()
{
    if (gripper_cfg::lager) {
        unloadColorFromSlotIfPresent(K_FARBE_BLAU);
    }
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_D, gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_L, gripper_cfg::SEIL_ROTATIONEN_HAUS_BLAU_GRUEN);
}

void AbladenModule::abladenGelb()
{
    if (gripper_cfg::lager) {
        unloadColorFromSlotIfPresent(K_FARBE_GELB);
    }
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_ROT_GELB_D, gripper_cfg::AUFNEHMEN_ABLEGEN_POS_ROT_GELB_L, gripper_cfg::SEIL_ROTATIONEN_HAUS_ROT_GELB);
}

void AbladenModule::abladenGruen()
{
    if (gripper_cfg::lager) {
        unloadColorFromSlotIfPresent(K_FARBE_GRUEN);
    }
    pickOrDropHouse(gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_D, gripper_cfg::AUFNEHMEN_ABLEGEN_POS_BLAU_GRUEN_L, gripper_cfg::SEIL_ROTATIONEN_HAUS_BLAU_GRUEN);
}
} // namespace abladen
