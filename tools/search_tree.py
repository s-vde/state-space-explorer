
import execution_tree as ext
import getopt
import os
import sys


def run(schedules, records_dir, output_dir):
    tree, schedules = ext.create_tree_from_schedules(schedules)
    
    traces = []
    operands_maps = []
    for root, dirs, files in os.walk(records_dir):
        short_records = filter(lambda filename : filename.startswith("record_short"), files)
        for short_record in short_records:
            trace, operands_map = ext.parse_trace(os.path.join(root, short_record))
            traces.append(trace)
            operands_maps.append(operands_map)
    
    for schedule, trace in zip(schedules, traces):
        ext.add_trace(tree, schedule, trace)
    
    ext.dump(tree, output_dir)
        
    ext.filter_tree(tree, schedules, traces, operands_maps, ["x[0][0]", "x[0][1]", "x[0][2]"])
    
    ext.dump(tree, os.path.join(output_dir, "pruned"))
    
    
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
    run (os.path.join(program_records_dir, "schedules.txt"), \
         os.path.join(program_records_dir, "records"), \
         output_dir)
    print ("==========")

if __name__ == "__main__":
    main(sys.argv[1:])
