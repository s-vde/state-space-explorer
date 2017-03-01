
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
    # tree.node_attr['fontname'] = 'Consolas'
    tree.edge_attr['arrowsize'] = '0.5'
    tree.edge_attr['weight'] = '100'
    tree.edge_attr['fontname'] = 'Ubuntu Code'
    tree.edge_attr['penwidth'] = 1
    tree.edge_attr['color'] = color
    tree.edge_attr['fontsize'] = fontsize
    
    return tree
    
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

def add_edge(tree, source_id, dest_id, thread_id, label):
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
        
def highlight_edge(tree, source_id, dest_id, color, penwidth):
    dummy_id = "_instr_%s" % dest_id
    source = tree.get_node(source_id)
    dummy = tree.get_node(dummy_id)
    dest = tree.get_node(dest_id)
    edge1 = tree.get_edge(source_id, dummy_id)
    edge2 = tree.get_edge(dummy_id, dest_id)
    source.attr['color'] = color
    dummy.attr['color'] = color
    dest.attr['color'] = color
    edge1.attr['color'] = color
    edge1.attr['penwidth'] = penwidth
    edge2.attr['color'] = color
    edge2.attr['penwidth'] = penwidth
    
#---------------------------------------------------------------------------------------------------
    
def reset_edge(tree, source_id, dest_id):
    highlight_edge(tree, source_id, dest_id, "black", 1)
    
#---------------------------------------------------------------------------------------------------

def add_schedule(tree, schedule):
    node_id = "s"
    add_node(tree, node_id)
    for thread_id in schedule:
        child_node_id = "%s.%d" % (node_id, thread_id)
        add_node(tree, child_node_id)
        add_edge(tree, node_id, child_node_id, thread_id, str(thread_id))
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

def highlight_schedule(tree, schedule, color="black"):
    node_id = "s"
    for thread_id in schedule:
        child_node_id = "%s.%d" % (node_id, thread_id)
        highlight_edge(tree, node_id, child_node_id, color, 3)
        node_id = child_node_id

#---------------------------------------------------------------------------------------------------

def reset_schedule(tree, schedule):
    node_id = "s"
    for thread_id in schedule:
        child_node_id = "%s.%d" % (node_id, thread_id)
        reset_edge(tree, node_id, child_node_id)
        node_id = child_node_id

#---------------------------------------------------------------------------------------------------

def add_execution(tree, execution):
    node_id = "s"
    add_node(tree, node_id)
    for (thread_id, instruction) in execution:
        child_node_id = "%s.%d" % (node_id, thread_id)
        add_node(tree, child_node_id)
        add_edge(tree, node_id, child_node_id, thread_id, instruction)
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
        add_edge(tree, node_id, child_node_id, thread_id, program[thread_id][thread_index])
        node_id = child_node_id
        thread_indices[thread_id] = thread_index + 1
    
#---------------------------------------------------------------------------------------------------

def dump(tree, output_dir, program_name):
    dot_dir = "%s/dot" % output_dir
    jpg_dir = "%s/jpg" % output_dir
    os.system("test -d %s || mkdir -p %s" % (dot_dir, dot_dir))
    os.system("test -d %s || mkdir -p %s" % (jpg_dir, jpg_dir))
    dot_name = "%s/dot/%s.dot" % (output_dir, program_name)
    # print ("dumping dot representation to %s" % dot_name)
    tree.write(dot_name)
    jpg_name = "%s/jpg/%s" % (output_dir, program_name)
    os.system("dot %s -Tjpg -o %s.jpg" % (dot_name, jpg_name))  

#---------------------------------------------------------------------------------------------------    

def parse_schedules(file_name):
    file = open(file_name,'r')
    lines = file.readlines()
    return map(lambda line : ast.literal_eval(line), lines)
    
#---------------------------------------------------------------------------------------------------

def parse(file_name):
    tree = execution_tree()
    schedules = parse_schedules(file_name)
    for schedule in schedules:
        add_schedule(tree, schedule)
    return tree

#---------------------------------------------------------------------------------------------------    
        