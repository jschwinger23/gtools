#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static void
handler(int sig) {
    printf("%d caught %d\n", getpid(), sig); // async signal insecure
    fflush(stdout);
    int orig_errno = errno;

    struct sigaction sa;
    sigset_t mask, prev_mask;

    if (signal(sig, SIG_DFL) == SIG_ERR)
        exit(-1);
    raise(sig);

    sigemptyset(&mask);
    sigaddset(&mask, sig);
    // SUSv3 specifies the following expression will resume the pending signal immediately
    if (sigprocmask(SIG_UNBLOCK, &mask, &prev_mask) == -1)
        exit(-1);

    // reach here only when receiving SIGCONT after SIGTSTP or SIGSTOP
    if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1)
        exit(-1);

    // reinstall
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(sig, &sa, NULL) == -1)
        exit(-1);

    printf("exiting sig handler\n");
    fflush(stdout);
    errno = orig_errno;
}

int
main(int argc, char *argv[]) {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;

    for (int i = 1; i < argc; i++) {
        int sig = atoi(argv[i]);
        if (sigaction(sig, &sa, NULL) == -1)
            exit(-1);
    }

    for (;;) {
        printf("%d waiting\n", getpid());
        fflush(stdout);
        pause();
    }
}

