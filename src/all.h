#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <sys/wait.h>
#include <dirent.h>

#include "store/store.h"
#include "utils/disk.h"
#include "ui/ui.h"
#include "ui/modal.h"
#include "ui/interfaces.h"
#include "steps/steps.h"
#include "steps/locale.h"
#include "steps/disk.h"
#include "steps/partition.h"
#include "steps/confirm.h"
#include "install/install.h"
