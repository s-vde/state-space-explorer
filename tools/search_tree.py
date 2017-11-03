
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
        ext.dump(tree, output_dir, str(index), export_formats)

        # reset the branch's font color
        ext.set_branch_color(tree, schedules[index], "black", "transparent")

# ------------------------------------------------------------------------------


def _create_tree_animation_tree_from(schedules, traces, statuses,
                                     operands_maps, name_filter):
    tree = ext.create_tree_from_schedules(schedules)
    for schedule, trace, status in zip(schedules, traces, statuses):
        ext.set_status(tree, schedule, status)
        ext.add_trace(tree, schedule, trace, status)
    if name_filter is not None:
        ext.apply_operand_names(tree, schedules, traces, operands_maps,
                                name_filter)
    return tree

# ------------------------------------------------------------------------------


def run(schedules, records_dir, output_dir, generate_animation=(False, None),
        name_filter=None, nodesep=3):
    tree, schedules = ext.create_tree_from_schedules_txt(schedules)
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

    # add status
    for schedule, status in zip(schedules, statuses):
        ext.set_status(tree, schedule, status)

    # schedules
    tree.graph_attr['nodesep'] = 2
    ext.dump(tree, output_dir, "full_schedules", export_formats)
    tree.graph_attr['nodesep'] = nodesep

    # add traces
    for schedule, trace, status in zip(schedules, traces, statuses):
        ext.add_trace(tree, schedule, trace, status)
    ext.dump(tree, output_dir, "full_traces", export_formats)

    # apply operand names
    if name_filter is not None:
        ext.apply_operand_names(tree, schedules, traces, operands_maps,
                                name_filter)
    ext.dump(tree, output_dir, "full_traces_nice", export_formats)

    if generate_animation[0]:
        if generate_animation[1] is None \
                or generate_animation[1] >= len(schedules):
            tree_ = tree
            schedules_ = schedules
            traces_ = traces
            operands_maps_ = operands_maps
        elif generate_animation[1] < len(schedules):
            schedules_ = schedules[0:generate_animation[1]]
            traces_ = traces[0:generate_animation[1]]
            statuses_ = statuses[0:generate_animation[1]]
            operands_maps_ = operands_maps[0:generate_animation[1]]
            tree_ = _create_tree_animation_tree_from(
                schedules_,
                traces_,
                statuses_,
                operands_maps_,
                name_filter)

        create_animation(tree_,
                         os.path.join(output_dir, "animations"),
                         export_formats,
                         schedules_,
                         traces_,
                         operands_maps_,
                         name_filter)

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------


def main(argv):
    try:
        opts, args = getopt.getopt(argv, "f:i:o:a:s:", [])
    except getopt.GetoptError:
        sys.exit(2)

    name_filter = None
    generate_animation = (False, None)
    nodesep = 3

    for opt, arg in opts:
        if opt == '-f':
            name_filter = ast.literal_eval(arg)
        if opt == '-i':
            program_records_dir = arg
        if opt == '-o':
            output_dir = arg
        if opt == '-a':
            # parse list
            if len(arg) >= 7 and arg[0:6] == "Until:":
                max_animation_index = int(arg[6:len(arg)])
                generate_animation = (True, max_animation_index)
            if arg == "True":
                generate_animation = (True, None)
        if opt == '-s':
            nodesep = int(arg)

    print ("==========\nGenerating search tree for %s" % (program_records_dir))
    print ("nodesep:\t%d" % nodesep)
    run(os.path.join(program_records_dir, "schedules.txt"),
        os.path.join(program_records_dir, "records"),
        output_dir,
        generate_animation=generate_animation,
        name_filter=name_filter,
        nodesep=nodesep)
    print ("==========")

# -----------------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])
