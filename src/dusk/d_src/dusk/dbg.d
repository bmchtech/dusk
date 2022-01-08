/* debug utils */
module dusk.dbg;

extern (C) nothrow @nogc:
version (DEBUG) {
    private {
        void mgba_printf(int level, const(char)* ptr, ...);
        void exit(int status);
    }

    pragma(inline, true) void ds_assert_func(bool cond, string msg, string file, int line) {
        if (cond)
            return;

        // report fatal error to mgba
        mgba_printf(0, "Assertion failed at %s:%d: %s\n", cast(char*) file, line, cast(char*) msg);

        exit(1);
    }

    void ds_assert(bool cond, string msg = "", string file = __FILE__, int line = __LINE__) {
        ds_assert_func(cond, msg, file, line);
    }
}
