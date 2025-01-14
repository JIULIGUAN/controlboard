﻿#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

static ITUTrackBar* settingSoundVolumeTrackBar;
static ITUTrackBar* settingSoundKeyVolumeTrackBar;

// status
static int settingSoundVolumeOld;
static int settingSoundKeyVolumeOld;

bool SettingSoundVolumeTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int percent = atoi(param);

    theConfig.audiolevel = percent;

    return true;
}

bool SettingSoundKeyVolumeTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int percent = atoi(param);

    theConfig.keylevel = percent;

    AudioSetKeyLevel(theConfig.keylevel);

    return true;
}

bool SettingSoundOnEnter(ITUWidget* widget, char* param)
{
    int value;

    if (!settingSoundVolumeTrackBar)
    {
        settingSoundVolumeTrackBar = ituSceneFindWidget(&theScene, "settingSoundVolumeTrackBar");
        assert(settingSoundVolumeTrackBar);

        settingSoundKeyVolumeTrackBar = ituSceneFindWidget(&theScene, "settingSoundKeyVolumeTrackBar");
        assert(settingSoundKeyVolumeTrackBar);
    }

    // current settings
    value = AudioGetVolume();
    ituTrackBarSetValue(settingSoundVolumeTrackBar, value);
    settingSoundVolumeOld = value;

    ituTrackBarSetValue(settingSoundKeyVolumeTrackBar, theConfig.keylevel);
    settingSoundKeyVolumeOld = theConfig.keylevel;

    return true;
}

bool SettingSoundOnLeave(ITUWidget* widget, char* param)
{
    if (settingSoundVolumeOld != AudioGetVolume() ||
        settingSoundKeyVolumeOld != theConfig.keylevel)
    {
        ConfigSave();
    }
    return true;
}

void SettingSoundReset(void)
{
    settingSoundVolumeTrackBar = NULL;
}
