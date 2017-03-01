
import execution_tree as ext
import ntpath

def run(file_name):
    tree = ext.parse(file_name)
    ext.dump(tree, "trees", ntpath.basename(file_name))
