#include "argtable3.h"
#include "cspammer.h"

#define MAX_PEM_LEN 4 * 1024
static cspammer_ctx_t* cspammer_ctx;

int cspammer_main(double *tps, const char** addr, const char** seed, const char** url, uint16_t *port, uint8_t https, uint8_t *mwm, uint32_t *depth, uint8_t *sec) {
    char *ca_pem = NULL;
    if (https) {
        ca_pem = malloc(MAX_PEM_LEN);
        FILE *f = fopen("pem.txt", "r");
        if (f == NULL) {
            printf("\nERROR[%s]: PEM file (%s) not found.\n", __FUNCTION__, "pem.txt");
            return -1;
        }
        fread(ca_pem, MAX_PEM_LEN, 1, f);
        fclose(f);
    }

    cspammer_ctx = cspammer_create(*url, *port, ca_pem, *depth, *mwm, *sec, *seed);
    if (cspammer_ctx == NULL) {
        printf("Err: Could not create CSpammer! \n");
        return -1;
    }

    char bundle_hash[NUM_TRYTES_BUNDLE + 1] = {};
    char *msg = "CSpammer be spammin'!";
    cspammer_err_t ret = cspammer_send(cspammer_ctx, *addr, 0, msg, bundle_hash);
    bundle_hash[NUM_TRYTES_BUNDLE] = '\0';
    log_info(cspammer_logger_id, "[%s:%d] Bundle hash: %s\n", __func__, __LINE__, bundle_hash);

    cspammer_destroy(&cspammer_ctx);
    free(ca_pem);

    return 0;
}

int main(int argc, char *argv[]) {

    struct arg_lit *help = arg_lit0(NULL,"help","print this help and exit");
    struct arg_str *addr = arg_str1("a", "addr", "<addr>", "Target IOTA Address Trytes");
    struct arg_dbl *tps = arg_dbl1("t", "tps", "<tps>", "Spamrate (Tx Per Second)");
    struct arg_str *seed = arg_str1("s", "seed", "<seed>", "Client Seed Trytes");
    struct arg_str *url = arg_str1("u", "url", "<url>", "Node's URL");
    struct arg_int *port = arg_int1("p", "port", "<port>", "Node's Port");
    struct arg_lit *https = arg_lit0(NULL, "https", "Node has HTTPS");
    struct arg_int *mwm = arg_int1("m", "mwm", "<mwm>", "Node Minimum Weight Magnitude (MWM)");
    struct arg_int *depth = arg_int1("d", "depth", "<depth>", "Node's Merkle Tree Depth");
    struct arg_int *sec = arg_int1("y", "sec", "<security>", "Node's Security Level");
    struct arg_end *end = arg_end(20);

    const char* progname = "cspammer";
    int nerrors;
    int exitcode=0;

    void* argtable[] = {help, addr, tps, seed, url, port, https, mwm, depth, sec, end};

    /* verify the argtable[] entries were allocated sucessfully */
    if (arg_nullcheck(argtable) != 0) {
        /* NULL entries were detected, some allocations must have failed */
        printf("%s: insufficient memory\n", progname);
        exitcode=1;
        goto exit;
    }

    /* Parse the command line as defined by argtable[] */
    nerrors = arg_parse(argc,argv,argtable);

    /* special case: '--help' takes precedence over error reporting */
    if (help->count > 0)
    {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout,argtable,"\n");
        printf("IOTA (Pre-Chrysalis pt.2) Client Spammer.\n\n");
        arg_print_glossary(stdout,argtable,"  %-10s %s\n");
        exitcode=0;
        goto exit;
    }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0)
    {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout,end,progname);
        printf("Try '%s --help' for more information.\n",progname);
        exitcode=1;
        goto exit;
    }

    exitcode = cspammer_main(tps->dval, addr->sval, seed->sval, url->sval, port->ival, https->count, mwm->ival, depth->ival, sec->ival);

exit:
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

    return exitcode;
}