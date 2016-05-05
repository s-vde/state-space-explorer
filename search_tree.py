#!/usr/bin/env python

import os

def op_to_string(op):
    return {
        'READ'      : 'r',
        'WRITE'     : 'w',
        'LOCK'      : 'lck',
        'UNLOCK'    : 'unlck',
    }.get(op,'NA')

class Instr( object ):
    def __init__( self ):
        self.tid = -1
        self.op = ""
        self.gvar = ""
        self.index = -1
    def op_to_string(self):
        if op == "READ":
            return 
    def to_string(self):
		index_str = "%s" % (self.index) if self.index > -1 else ""
		return "%s %s %s %s" % (self.tid, op_to_string(self.op), self.gvar, index_str)

class TreeData(object):
    def __init__(self):
        self.instr = Instr()
        self.next = dict()
        self.ssb = False
        self.cs = False
        self.pr = False
        self.boundv = 0 # preemption bound
    def set_boundv(self, parent):
        if not parent == None:
            previous = parent.data.instr
            if not previous.tid == self.instr.tid:
                self.cs = True
                if previous.tid in parent.data.next:
                    (I,en) = parent.data.next[previous.tid]
                    if en:
                        self.pr = True
            self.boundv = parent.data.boundv + 1 if self.pr else parent.data.boundv
    def node_to_dot(self, id, _bv):
        shape = "shape=circle" if not self.ssb else "shape=doublecircle"
        size = "width=0.2, height=0.2" if not _bv else "width=0.2, height=0.2"
        fill = "style=filled, color=black" if not _bv else "style=draw"
        style = "%s, %s, %s" % (shape, fill, size)
        label = "" if not _bv else "\"%s\"" % (self.boundv)
        return "\tn%d [fixedsize=true, %s, label=%s];" % (id, style, label)
    def edge_label(self):
        return self.instr.to_string()

class Tree(object):
    def __init__( self ):
        self.id = -1
        self.children = []
        self.parent = None
        self.data = TreeData()
    # Looksup whether this tree has a child with given tid
    # and if so returns this child.
    def has_child(self, tid):
        for child in self.children:
            if child.data.instr.tid == tid:
                return child
        return None
    def edge_to_dot(self, src):
        return "\tn%s -- n%s [label=\"%s\"];" % (src, self.id, self.data.edge_label())
    '''def children_order_constraint(self):
        if len(self.children) > 1:
            ids = map(lambda child: "n%d" % (child.id), self.children)
            return "\t%s %s" % (" -> ".join(ids), "[];")
        return ""'''
    def to_dot(self, ptf):
        os.system(("touch %s.dot") % ptf)
        dotfile = open("%s.dot" % ptf,'w')

        header = "graph searchtree {\n\tsize=\"8.3,11.7\"\n"
        footer = "}"
        print >> dotfile, header
        #print header
        
        stack = [self]
        while len(stack) > 0:
            T = stack.pop()
            print >> dotfile, T.data.node_to_dot(T.id, True)
            '''print >> dotfile, T.children_order_constraint()'''
            # print T.data.node_to_dot(T.id, True)
            if not T.parent == None:
                print >> dotfile, T.edge_to_dot(T.parent.id)
                #print T.edge_to_dot(T.parent.id)
            stackAA = []
            for child in T.children:
                stackAA.append(child)
            while (len(stackAA) > 0):
                child = stackAA.pop()
                stack.append(child)
        print >> dotfile, footer
        #print footer
        dotfile.close()

def parse_instr(li):
    I = Instr()
    I.tid = li[1]
    I.op = li[2]
    I.gvar = li[3]
    I.index = li[4]
    return I

def parse_next(li):
    I = Instr()
    I.tid = li[0]
    I.op = li[1]
    I.gvar = li[3]
    I.index = li[4]
    enabled = (li[5] == ":1")
    return (I,enabled)

def tree_to_dot(dir):
    # Output directory and output file
    dotfile = "%s/searchtree" % (dir)

    #=====
    print "Creating searchtree\ndir:\t%s\n" % (dir)
    #=====

    T = Tree()
    nextid = 0
	
    for root, dirs, files in os.walk(dir):
        for file in files:
            if not ("exploration" in file or "schedules" in file or ".DS_Store" in file or "record_short" in file or "statistics" in file) :
                #=====
                print "Reading %s" % (file)
                #=====
                T_ = T
                
                file_ = open("%s/%s" % (dir, file),'r')
                lines = file_.readlines()
                mode_instr = False
                duplicate = True;
                last = False;
                last_node = None;
                # Alternately read
                for line in lines:
					# Check if SSB
					if last:
						if line == "BLOCKED\n": last_node.ssb = True
					# Check for end
					elif line == "=====\n": last = True;
                    # Parse
					else:
						ins = line.split()
						if mode_instr:
							C = T_.has_child(ins[1])
							if C == None:
								C = Tree()
								C.id = nextid
								C.parent = T_
								C.data = TreeData()
								C.data.instr = parse_instr(ins)
								C.data.set_boundv(T_)
								T_.children.append(C)
								last_node = C;
								nextid = nextid + 1
								duplicate = False;
							T_ = C
							mode_instr = False
						else:
							#if len(T_.children) == 0:
							#    (I,en) = parse_next(ins)
							#    T_.data.next[I.tid] = (I,en)
							mode_instr = True
					file_.close()
						#if duplicate:
						#print "Duplicate found"
						#return
    T.to_dot(dotfile)
