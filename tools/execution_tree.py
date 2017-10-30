
import ast
import os
import pygraphviz as pgv


#---------------------------------------------------------------------------------------------------

def execution_tree(color='black', nodesep='3'):
    tree = pgv.AGraph(strict=False,directed=True)
    
    fontsize = '22'
    
    tree.graph_attr['dpi'] = '300'
    tree.graph_attr['strict'] = 'False'
    tree.graph_attr['directed'] = 'True' 
    tree.graph_attr['rankdir'] = 'TB'      # vertical edge direction
    tree.graph_attr['ranksep'] = '0.3'
    tree.graph_attr['nodesep'] = nodesep
    tree.graph_attr['ordering'] = 'out'
    tree.graph_attr['forcelabels'] = 'True'
    # node style
    tree.node_attr['shape'] = 'point'
    tree.node_attr['fixedsize'] = 'true'
    tree.node_attr['width'] = '.05'
    tree.node_attr['height'] = '.05'
    tree.node_attr['style'] = 'filled'
    tree.node_attr['fillcolor'] = 'black'
    tree.node_attr['color'] = color
    tree.node_attr['fontsize'] = fontsize
    # edge style
    tree.edge_attr['arrowsize'] = '0.5'
    tree.edge_attr['weight'] = '100'
    tree.edge_attr['penwidth'] = 1
    tree.edge_attr['color'] = color
    tree.edge_attr['fontsize'] = fontsize
    
    return tree


#---------------------------------------------------------------------------------------------------
# node
#---------------------------------------------------------------------------------------------------

def node_of_schedule(schedule, prefix="s"):
    if len(schedule) > 0:
        return "%s.%s" % (prefix, 
                        ".".join(map(lambda thread_id : str(thread_id), schedule)))
    return prefix
    
#---------------------------------------------------------------------------------------------------

def dummy_node_of_schedule(schedule, prefix="s"):
    return "_instr_%s" % node_of_schedule(schedule, prefix)
    
#---------------------------------------------------------------------------------------------------

def add_node(tree, node_id):
    if not tree.has_node(node_id):
        tree.add_node(node_id)
        node = tree.get_node(node_id)
        node.attr['label'] = ""
        dummy_instr_id = "_instr_%s" % node_id
        tree.add_node(dummy_instr_id, width='0')

#---------------------------------------------------------------------------------------------------

def set_node_fontcolor(tree, node_id, fontcolor):
    node = tree.get_node(node_id)
    node.attr['fontcolor'] = fontcolor
    
#---------------------------------------------------------------------------------------------------

def set_node_shape(tree, node_id, shape, width, height, fillcolor):
    node = tree.get_node(node_id)
    node.attr['shape'] = shape
    node.attr['width'] = width
    node.attr['height'] = height
    node.attr['fillcolor'] = fillcolor
    
#---------------------------------------------------------------------------------------------------

def reset_node_shape(tree, node_id):
    set_node_shape(tree, node_id, 
                   tree.node_attr['shape'], 
                   tree.node_attr['width'], 
                   tree.node_attr['height'],
                   tree.node_attr['fillcolor'])

#---------------------------------------------------------------------------------------------------

def add_to_node_label(tree, node_id, label, color="black"):
    node = tree.get_node(node_id)
    node.attr['xlabel'] = "  %s  %s  " % (node.attr['xlabel'], label)
    node.attr['fontcolor'] = color


#---------------------------------------------------------------------------------------------------
# edge        
#---------------------------------------------------------------------------------------------------

def add_edge(tree, source_id, dest_id, label):
    dummy_id = "_instr_%s" % dest_id
    if not tree.has_edge(source_id, dummy_id):
        tree.add_edge(source_id, dummy_id, dir='none')
        tree.add_edge(dummy_id, dest_id)
    dummy_node = tree.get_node(dummy_id)
    dummy_node.attr['xlabel'] = ("  %s  " % label)

#---------------------------------------------------------------------------------------------------

def remove_edge_and_nodes(tree, src_id, dst_id, next_id=None):
    dummy_dst_id = "_instr_%s" % dst_id
    if tree.has_edge(src_id, dummy_dst_id):
        tree.remove_edge(src_id, dummy_dst_id)
    if tree.has_edge(dummy_dst_id, dst_id):
        tree.remove_edge(dummy_dst_id, dst_id)
    tree.remove_node(dummy_dst_id)
    tree.remove_node(dst_id)
    
    if not next_id == None:
        dummy_next_id = "_instr_%s" % next_id
        tree.add_edge(src_id, dummy_next_id, dir='none')


#---------------------------------------------------------------------------------------------------
# highlighting
#---------------------------------------------------------------------------------------------------
    
def set_edge_color(tree, source_id, dest_id, color, fontcolor, penwidth):
    dummy_id = "_instr_%s" % dest_id
    
    nodes = [tree.get_node(source_id), tree.get_node(dummy_id), tree.get_node(dest_id)]
    for node in nodes:
        node.attr['color'] = color
        node.attr['fillcolor'] = color
        node.attr['fontcolor'] = fontcolor
        
    edges = [tree.get_edge(source_id, dummy_id), tree.get_edge(dummy_id, dest_id)]
    for edge in edges:
        edge.attr['color'] = color
        edge.attr['penwidth'] = penwidth
    
#---------------------------------------------------------------------------------------------------

def set_branch_color(tree, schedule, color, fontcolor=None):
    if fontcolor == None:
        fontcolor = color
    
    node_id = "s"
    for thread_id in schedule:
        child_node_id = "%s.%d" % (node_id, thread_id)
        set_edge_color(tree, node_id, child_node_id, color, fontcolor, 3)
        node_id = child_node_id


#---------------------------------------------------------------------------------------------------
# schedule    
#---------------------------------------------------------------------------------------------------

def add_schedule(tree, schedule):
    node_id = "s"
    add_node(tree, node_id)
    for thread_id in schedule:
        child_node_id = "%s.%d" % (node_id, thread_id)
        add_node(tree, child_node_id)
        add_edge(tree, node_id, child_node_id, str(thread_id))
        node_id = child_node_id
        
#---------------------------------------------------------------------------------------------------

def remove_schedule(tree, schedule, from_index):
    node_id = "s"
    index = 1
    for thread_id in schedule:
        child_node_id = "%s.%d" % (node_id, thread_id)
        if index >= from_index:
            remove_edge_and_nodes(tree, node_id, child_node_id)
        index = index+1
        node_id = child_node_id

#---------------------------------------------------------------------------------------------------    

def parse_schedule(schedule):
    schedule = schedule.replace("<", "[")
    schedule = schedule.replace(">", "]")
    # parse schedule as a list
    return ast.literal_eval(schedule)

#---------------------------------------------------------------------------------------------------    

def parse_schedules(file_name):
    file = open(file_name,'r')
    lines = file.readlines()
    return list(map(lambda line : parse_schedule(line), lines))


#---------------------------------------------------------------------------------------------------
# dump
#---------------------------------------------------------------------------------------------------

def dump(tree, output_dir):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    dot_file = os.path.join(output_dir, "search_tree.dot")
    print ("dumping dot representation to %s" % dot_file)
    tree.write(dot_file)
    
    eps_file = os.path.join(output_dir, "search_tree.eps")
    print ("dumping eps representation to %s" % eps_file)
    os.system("dot %s -Teps -o %s" % (dot_file, eps_file))  


#---------------------------------------------------------------------------------------------------
# trace
#---------------------------------------------------------------------------------------------------

def add_trace(tree, schedule, trace):
    for index in range(0, len(schedule)):
        instruction = trace[index]
        instruction_str = "  %s %s %s  " % (instruction[0], instruction[1], instruction[3])
        node = tree.get_node(dummy_node_of_schedule(schedule[0: index+1]))
        node.attr['xlabel'] = instruction_str

#---------------------------------------------------------------------------------------------------

def parse_instruction(line, operands):
    instruction = line.split(" ")[1:]
    # remove undesired characters
    instruction = list(map(lambda entry : entry.replace("\"", "").replace("\n", ""), instruction))
    address = instruction[2]
    name = instruction[3]
    if address not in operands:
        operands[address] = []
    if name not in operands[address]:
        operands[address].append(name)
    return instruction
    
#---------------------------------------------------------------------------------------------------

def parse_trace(file_name):
    file = open(file_name,'r')
    lines = file.readlines()
    operands = dict()
    execution = []
    for line in lines[0:len(lines)-1]:
        execution.append(parse_instruction(line, operands))
    return (execution, operands)

# -----------------------------------------------------------------------------
# filter tree
# -----------------------------------------------------------------------------


def filter_tree(tree, schedules, traces, operands_maps, selected_operands):
    operands_lists = \
        list(map(lambda operand_map:
                 {address for address, names in operand_map.items()
                  if (len(list(filter(lambda name:
                                      name.split('[')[0] in selected_operands,
                                      names)))) > 0},
                 operands_maps))

    for schedule, trace, operands_list in zip(schedules, traces,
                                              operands_lists):
        src_id = node_of_schedule(schedule[0: 0])
        for index in range(0, len(schedule)):
            dst_id = node_of_schedule(schedule[0: index+1])
            if tree.has_node(src_id) and tree.has_node(dst_id):
                if not trace[index][2] in operands_list:
                    if index < len(schedule)-1:
                        remove_edge_and_nodes(tree, src_id, dst_id,
                                              node_of_schedule(schedule[0: index+2]))
                    else:
                        remove_edge_and_nodes(tree, src_id, dst_id)
                else:
                    src_id = dst_id

# -----------------------------------------------------------------------------

def create_tree_from_schedules(file_name):
    tree = execution_tree()
    schedules = parse_schedules(file_name)
    for schedule in schedules:
        add_schedule(tree, schedule)
    return (tree, schedules)
