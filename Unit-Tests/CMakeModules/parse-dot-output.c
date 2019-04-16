/*
 * gvpr script
 * ===========
 *
 * Discover a list of tests from Boost.Test.  This script parses the DOT output
 * from Boost.Test's --list_content command line option.
 *
 */
BEGIN {
    int i, indent;
    int seen[string];
    string breadcrumb[int];
    void print_breadcrumbs (int cnt) {
        for (i = 1; i < cnt-1; i++) {
            printf ("%s/",breadcrumb[i]);
        }
        print(breadcrumb[cnt-1]);
    }
}
BEG_G {
    $tvtype = TV_prepostfwd;
    /* $tvroot = node($,ARGV[0]); */
}
N {
    if (seen[$.name]) indent--;
    else {
        string labelsplit[int];
        split($.label, labelsplit, "|");
        breadcrumb[indent] = labelsplit[0];
        seen[$.name] = 1;
        indent++;
        if ($.outdegree == 0) {
            print_breadcrumbs(indent);
        }
    }
}
