#ifndef _GUI_
#define _GUI_

#ifndef GUI_STANDALONE
#define GUIAssert(expression, description) RTEAssert(expression, description)
#define GUIAbort(description) RTEAbort(description)
#else
#define GUIAssert(expression, description) assert(expression)
#define GUIAbort(description) abort()
#endif

#include "GUIInterface.h"
#include "GUIProperties.h"
#include "GUIInput.h"
#include "GUIFont.h"
#include "GUISkin.h"
#include "GUIUtil.h"
#include "GUIControl.h"
#include "GUIEvent.h"
#include "GUIControlManager.h"

#ifndef GUI_STANDALONE
#include "GUISound.h"
#endif

#endif