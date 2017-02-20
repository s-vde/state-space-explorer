
import execution_tree
import ntpath

def run(file_name):
    tree = execution_tree.parse(file_name)
    execution_tree.dump(tree, "trees", ntpath.basename(file_name))
