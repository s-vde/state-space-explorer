
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

def node_of_schedule(schedule, prefix="s."):
    return "%s%s" % (prefix, 
                      ".".join(map(lambda thread_id : str(thread_id), schedule)))
    
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

def remove_edge_and_nodes(tree, source_id, dest_id):
    dummy_id = "_instr_%s" % dest_id
    tree.remove_edge(source_id, dummy_id)
    tree.remove_edge(dummy_id, dest_id)
    tree.remove_node(dummy_id)
    tree.remove_node(dest_id)


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
# execution
#---------------------------------------------------------------------------------------------------    

def add_execution(tree, execution):
    node_id = "s"
    add_node(tree, node_id)
    for (thread_id, instruction) in execution:
        child_node_id = "%s.%d" % (node_id, thread_id)
        add_node(tree, child_node_id)
        add_edge(tree, node_id, child_node_id, instruction)
        node_id = child_node_id
        
#---------------------------------------------------------------------------------------------------

def add_execution_from_program_and_schedule(tree, program, schedule):
    node_id = "s"
    add_node(tree, node_id)
    thread_indices = list(map(lambda thread_id : 0, program))
    for thread_id in schedule:
        thread_index = thread_indices[thread_id]
        child_node_id = "%s.%d" % (node_id, thread_id)
        add_node(tree, child_node_id)
        add_edge(tree, node_id, child_node_id, program[thread_id][thread_index])
        node_id = child_node_id
        thread_indices[thread_id] = thread_index + 1


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

def create_tree_from_schedules(file_name):
    tree = execution_tree()
    schedules = parse_schedules(file_name)
    for schedule in schedules:
        add_schedule(tree, schedule)
    return (tree, schedules)
