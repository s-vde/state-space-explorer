
import ast
import execution_tree as ext
import getopt
import os
import re
import sys


def atoi(text):
    return int(text) if text.isdigit() else text

# -----------------------------------------------------------------------------


def natural_keys(text):
    '''
    alist.sort(key=natural_keys) sorts in human order
    http://nedbatchelder.com/blog/200712/human_sorting.html
    (See Toothy's implementation in the comments)
    '''
    return [atoi(c) for c in re.split('(\d+)', text)]

# -----------------------------------------------------------------------------


def run(schedules, records_dir, output_dir, name_filter=None):
    tree, schedules = ext.create_tree_from_schedules(schedules)

    traces = []
    operands_maps = []
    statuses = []
    for root, dirs, files in os.walk(records_dir):
        short_records = list(filter(lambda filename:
                                    filename.startswith("record_short"),
                                    files))
        short_records.sort(key=natural_keys)
        for short_record in short_records:
            trace, operands_map, status = ext.parse_trace(os.path.join(root, short_record))
            traces.append(trace)
            operands_maps.append(operands_map)
            statuses.append(status)

    for schedule, trace, status in zip(schedules, traces, statuses):
        ext.add_trace(tree, schedule, trace, status)

    ext.dump(tree, output_dir)

    if name_filter is not None:
        ext.filter_tree(tree, schedules, traces, operands_maps, name_filter)

    ext.dump(tree, os.path.join(output_dir, "pruned"))

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------


def main(argv):
    try:
        opts, args = getopt.getopt(argv, "f:i:o:", [])
    except getopt.GetoptError:
        sys.exit(2)

    name_filter = None

    for opt, arg in opts:
        if opt == '-f':
            name_filter = ast.literal_eval(arg)
        if opt == '-i':
            program_records_dir = arg
        if opt == '-o':
            output_dir = arg

    print ("==========\nGenerating search tree for %s" % (program_records_dir))
    run(os.path.join(program_records_dir, "schedules.txt"),
        os.path.join(program_records_dir, "records"),
        output_dir,
        name_filter)
    print ("==========")

# -----------------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])
