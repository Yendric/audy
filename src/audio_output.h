#pragma once
#include <windows.h>

// Makes the next active output device the default. Expects COM to be initialized.
HRESULT CycleAudioOutput(void);
