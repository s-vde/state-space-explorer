
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

# ------------------------------------------------------------------------------
# play
# ------------------------------------------------------------------------------


def create_animation(tree, output_dir, export_formats, schedules, traces,
                     operands_maps, name_filter=None):
    for schedule in schedules:
        ext.set_branch_color(tree, schedule, "transparent", "transparent")

    for index in range(0, len(schedules)):
        ext.set_branch_color(tree, schedules[index], "black")
        ext.dump(tree, os.path.join(output_dir, "full"), str(index),
                 export_formats)

        # name filtering
        tree_ = tree.copy()
        if name_filter is not None:
            ext.prune_tree(tree_, schedules, traces, operands_maps,
                           name_filter)
        ext.dump(tree_, os.path.join(output_dir, "pruned"), str(index),
                 export_formats)

        # reset the branch's font color
        ext.set_branch_color(tree, schedules[index], "black", "transparent")

# ------------------------------------------------------------------------------


def run(schedules, records_dir, output_dir, name_filter=None, nodesep=3):
    tree, schedules = ext.create_tree_from_schedules(schedules)
    tree.graph_attr['nodesep'] = nodesep

    traces = []
    operands_maps = []
    statuses = []
    for root, dirs, files in os.walk(records_dir):
        short_records = list(filter(lambda filename:
                                    filename.startswith("record_short"),
                                    files))
        short_records.sort(key=natural_keys)
        for short_record in short_records:
            trace, operands_map, status = \
                ext.parse_trace(os.path.join(root, short_record))
            traces.append(trace)
            operands_maps.append(operands_map)
            statuses.append(status)

    export_formats = ["eps", "png"]

    # schedules
    tree.graph_attr['nodesep'] = 1
    ext.dump(tree, output_dir, "full_schedules", export_formats)
    tree.graph_attr['nodesep'] = 3

    # add traces
    for schedule, trace, status in zip(schedules, traces, statuses):
        ext.add_trace(tree, schedule, trace, status)
    ext.dump(tree, output_dir, "full_traces", export_formats)

    create_animation(tree,
                     os.path.join(output_dir, "animations"),
                     export_formats,
                     schedules,
                     traces,
                     operands_maps,
                     name_filter)

    # apply name filter
    if name_filter is not None:
        ext.prune_tree(tree, schedules, traces, operands_maps, name_filter)
    ext.dump(tree, output_dir, "full_pruned", export_formats)

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------


def main(argv):
    try:
        opts, args = getopt.getopt(argv, "f:i:o:s:", [])
    except getopt.GetoptError:
        sys.exit(2)

    name_filter = None
    nodesep = 3

    for opt, arg in opts:
        if opt == '-f':
            name_filter = ast.literal_eval(arg)
        if opt == '-i':
            program_records_dir = arg
        if opt == '-o':
            output_dir = arg
        if opt == '-s':
            nodesep = int(arg)

    print ("==========\nGenerating search tree for %s" % (program_records_dir))
    print ("nodesep:\t%d" % nodesep)
    run(os.path.join(program_records_dir, "schedules.txt"),
        os.path.join(program_records_dir, "records"),
        output_dir,
        name_filter=name_filter,
        nodesep=nodesep)
    print ("==========")

# -----------------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])
