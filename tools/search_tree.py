
import execution_tree as ext
import getopt
import os
import sys


def run(schedules, output_dir):
    tree, schedules = ext.create_tree_from_schedules(schedules)
    ext.dump(tree, output_dir)
    
#---------------------------------------------------------------------------------------------------    
# main
#---------------------------------------------------------------------------------------------------    

def main(argv):
    try:
      opts, args = getopt.getopt(argv, "i:o:",[])
    except getopt.GetoptError:
      sys.exit(2)
    
    traces = []
    
    for opt, arg in opts:
      if opt == '-i':
         program_records_dir = arg
      if opt == '-o':
         output_dir = arg
    
    print ("==========\nGenerating search tree for %s" % (program_records_dir))
    run (os.path.join(program_records_dir, "schedules.txt"), output_dir)
    print ("==========")

if __name__ == "__main__":
    main(sys.argv[1:])
