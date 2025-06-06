#include <3ds.h>
#include <stdio.h>
#include <stdarg.h>

void logw(const char *format, ...);

static bool task_quit;

static Handle task_pause_event;
static Handle task_suspend_event;

static aptHookCookie cookie;

static void task_apt_hook(APT_HookType hook, void* param) {
			logw("apt hook=%d\n", hook);
	switch(hook) {

		case APTHOOK_ONSUSPEND:
			svcClearEvent(task_suspend_event);
			break;

		case APTHOOK_ONRESTORE:
			svcSignalEvent(task_suspend_event);
			break;

		case APTHOOK_ONSLEEP:
			svcClearEvent(task_pause_event);
			break;

		case APTHOOK_ONWAKEUP:
			logw("received wake up signal\n ");
			svcSignalEvent(task_pause_event);
			break;

		case APTHOOK_ONEXIT:
			logw("received exit signal\n");
			break;

		default:
			break;
	}
}

void notif() {
	/* Result srvEnableNotification(Handle* semaphoreOut); */
	Result srvSubscribe(u32 notificationId);
}

void task_init() {
	notif();
	task_quit = false;

	Result res = 0;

	aptSetHomeAllowed(true);
	aptSetSleepAllowed(true);

	if(R_FAILED(res = svcCreateEvent(&task_pause_event, RESET_STICKY))) {
		logw("Failed to create task pause event: 0x%08lX\n", res);
		return;
	}

	if(R_FAILED(res = svcCreateEvent(&task_suspend_event, RESET_STICKY))) {
		svcCloseHandle(task_pause_event);
		logw("Failed to create task suspend event: 0x%08lX\n", res);
		return;
	}

	svcSignalEvent(task_pause_event);
	svcSignalEvent(task_suspend_event);

	aptHook(&cookie, task_apt_hook, NULL);
	logw("register apt hooks \n ");
}

void task_exit() {
	task_quit = true;

	aptUnhook(&cookie);

	if(task_pause_event != 0) {
		svcCloseHandle(task_pause_event);
		task_pause_event = 0;
	}

	if(task_suspend_event != 0) {
		svcCloseHandle(task_suspend_event);
		task_suspend_event = 0;
	}
}

bool task_is_quit_all() {
	return task_quit;
}

Handle task_get_pause_event() {
	return task_pause_event;
}

Handle task_get_suspend_event() {
	return task_suspend_event;
}