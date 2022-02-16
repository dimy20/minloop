#define mu_assert(test, message)    \
    do {                            \
        if (!(test)) {              \
            log_err(message);       \
            return message;         \
        }                           \
    } while(0)

#define mu_run_test(test)                   \
    do {                                    \
        printf("\n-----%s", " " #test);     \
        message = test();                   \
        tests_run++;                        \
        if (message) { return message; }    \
    while(0)

 extern int tests_run;

