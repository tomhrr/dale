; ModuleID = 't/src/vector-ins-erase.dt'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32"

%struct__Z8MContext = type { i32, %struct__Z8PoolNode*, %struct__Z3_vp* }
%struct__Z8PoolNode = type { %struct__Z3_vp*, %struct__Z8PoolNode*, %struct__Z8PoolNode* }
%struct__Z3_vp = type { i8 }
%struct__Z5DNode = type { i32, i8*, %struct__Z5DNode*, %struct__Z5DNode*, i32, i32, i32, i32, i32, i32, i32, i32, i8* }
%"struct__Z7va$2dlist" = type { i32 }
%struct__Z4file = type opaque
%"struct__ZN3std8concepts12concept$2dnodeE" = type { i8*, i32, %"struct__ZN3std8concepts12concept$2dnodeE"** }
%"struct__Z11buffer$2dmode" = type { i32 }
%struct__Z11fseekorigin = type { i32 }
%struct__Z4fpos = type { [12 x i8] }
%"struct__Z5div$2dt" = type { i32, i32 }
%struct__Z9Container = type { i32 }
%struct__Z15TrivialIterator = type { i32 }
%struct__Z14OutputIterator = type { i32 }
%struct__Z26SimpleAssociativeContainer = type { i32 }
%struct__Z26SortedAssociativeContainer = type { i32 }
%struct__Z26UniqueAssociativeContainer = type { i32 }
%struct__Z24PairAssociativeContainer = type { i32 }
%struct__Z20RandomAccessIterator = type { i32 }
%struct__Z16ForwardContainer = type { i32 }
%struct__Z20AssociativeContainer = type { i32 }
%struct__Z19ReversibleContainer = type { i32 }
%struct__Z8Sequence = type { i32 }
%struct__Z21BidirectionalIterator = type { i32 }
%struct__Z15ForwardIterator = type { i32 }
%struct__Z21RandomAccessContainer = type { i32 }
%struct__Z22FrontInsertionSequence = type { i32 }
%struct__Z21BackInsertionSequence = type { i32 }
%struct__Z13InputIterator = type { i32 }
%struct__Z6Struct = type { i32 }
%struct__Z10Assignable = type { i32 }
%struct__Z20DefaultConstructible = type { i32 }
%struct__Z18EqualityComparable = type { i32 }
%struct__Z18LessThanComparable = type { i32 }
%"struct__ZN3std7div$2dintE" = type { i32, i32 }
%"struct__ZN3std9div$2dint16E" = type { i16, i16 }
%"struct__ZN3std9div$2dint32E" = type { i32, i32 }
%"struct__ZN3std9div$2dint64E" = type { i64, i64 }
%struct__Z7vectori = type { i32*, i32*, i32, i32 }
%struct__Z17iteratorZ7vectori = type { %struct__Z7vectori*, i32* }
%struct__Z18riteratorZ7vectori = type { i32* }

@mc = external global %struct__Z8MContext*
@CLOCK_T = external global i32
@DBL_DIG = external global i32
@DBL_EPSILON = external global double
@DBL_MANT_DIG = external global i32
@DBL_MAX = external global double
@DBL_MAX_EXP = external global i32
@DBL_MIN = external global double
@DBL_MIN_EXP = external global i32
@EXIT_FAILURE = external global i32
@EXIT_SUCCESS = external global i32
@FILENAME_MAX = external global i32
@FLT_DIG = external global i32
@FLT_EPSILON = external global float
@FLT_MANT_DIG = external global i32
@FLT_MAX = external global float
@FLT_MAX_EXP = external global i32
@FLT_MIN = external global float
@FLT_MIN_EXP = external global i32
@FLT_RADIX = external global i32
@FLT_ROUNDS = external global i32
@FOPEN_MAX = external global i32
@FPOS_T = external global i32
@HUGE_VAL = external global double
@HUGE_VALF = external global float
@HUGE_VALL = external global x86_fp80
@JMP_BUF_SIZE = external global i32
@LDBL_DIG = external global i32
@LDBL_EPSILON = external global x86_fp80
@LDBL_MANT_DIG = external global i32
@LDBL_MAX = external global x86_fp80
@LDBL_MAX_EXP = external global i32
@LDBL_MIN = external global x86_fp80
@LDBL_MIN_EXP = external global i32
@L_tmpnam = external global i32
@RAND_MAX = external global i32
@SIZEOF_LONG = external global i32
@SIZEOF_LONG_LONG = external global i32
@TIME_T = external global i32
@TMP_MAX = external global i32
@n = external global i32
@str = external global i8*
@str2 = external global i8*
@frm = external global %struct__Z5DNode*
@T = external global %struct__Z5DNode*
@_dv203 = external global [5 x i8]
@_dv204 = external global [12 x i8]
@_dv205 = external global [3 x i8]
@_dv206 = external global [2 x i8]
@_dv207 = external global [5 x i8]
@_dv208 = external global [5 x i8]
@_dv209 = external global [7 x i8]
@_dv210 = external global [2 x i8]
@"_ZN3std6macros8itoa$2dstrE" = external global [50 x i8]
@"_ZN3std6macros12label$2dmarkerE" = external global i32
@"_ZN3std6macros10var$2dmarkerE" = external global i32
@_dv100 = external global [10 x i8]
@_dv101 = external global [10 x i8]
@_dv102 = external global [10 x i8]
@_dv10 = external global [2 x i8]
@_dv11 = external global [8 x i8]
@_dv12 = external global [16 x i8]
@_dv13 = external global [3 x i8]
@_dv14 = external global [6 x i8]
@_dv15 = external global [19 x i8]
@_dv16 = external global [10 x i8]
@_dv17 = external global [8 x i8]
@_dv5 = external global [3 x i8]
@_dv6 = external global [5 x i8]
@_dv7 = external global [2 x i8]
@_dv8 = external global [10 x i8]
@_dv9 = external global [5 x i8]
@_dv19 = external global [3 x i8]
@_dv20 = external global [5 x i8]
@_dv21 = external global [2 x i8]
@_dv22 = external global [10 x i8]
@_dv23 = external global [5 x i8]
@_dv24 = external global [2 x i8]
@_dv25 = external global [8 x i8]
@_dv26 = external global [5 x i8]
@_dv27 = external global [3 x i8]
@_dv28 = external global [5 x i8]
@_dv29 = external global [19 x i8]
@_dv30 = external global [10 x i8]
@_dv31 = external global [8 x i8]
@_dv32 = external global [4 x i8]
@_dv33 = external global [4 x i8]
@_dv34 = external global [5 x i8]
@_dv35 = external global [2 x i8]
@_dv36 = external global [4 x i8]
@_dv37 = external global [10 x i8]
@_dv38 = external global [10 x i8]
@_dv39 = external global [5 x i8]
@_dv40 = external global [2 x i8]
@_dv41 = external global [2 x i8]
@_dv42 = external global [10 x i8]
@_dv43 = external global [10 x i8]
@_dv44 = external global [4 x i8]
@_dv45 = external global [8 x i8]
@_dv46 = external global [10 x i8]
@_dv47 = external global [5 x i8]
@_dv48 = external global [2 x i8]
@_dv49 = external global [2 x i8]
@_dv50 = external global [14 x i8]
@_dv51 = external global [8 x i8]
@_dv52 = external global [10 x i8]
@vn = external global %struct__Z5DNode*
@_dv54 = external global [3 x i8]
@_dv55 = external global [5 x i8]
@_dv56 = external global [2 x i8]
@_dv57 = external global [10 x i8]
@_dv58 = external global [5 x i8]
@_dv59 = external global [2 x i8]
@_dv60 = external global [8 x i8]
@_dv61 = external global [19 x i8]
@_dv62 = external global [10 x i8]
@_dv63 = external global [8 x i8]
@_dv65 = external global [3 x i8]
@_dv66 = external global [5 x i8]
@_dv67 = external global [2 x i8]
@_dv68 = external global [10 x i8]
@_dv69 = external global [5 x i8]
@_dv70 = external global [2 x i8]
@_dv71 = external global [8 x i8]
@_dv72 = external global [19 x i8]
@_dv73 = external global [10 x i8]
@_dv74 = external global [8 x i8]
@_dv75 = external global [4 x i8]
@_dv76 = external global [6 x i8]
@_dv77 = external global [4 x i8]
@_dv78 = external global [5 x i8]
@_dv79 = external global [2 x i8]
@_dv80 = external global [2 x i8]
@_dv81 = external global [2 x i8]
@_dv82 = external global [2 x i8]
@_dv83 = external global [10 x i8]
@_dv84 = external global [10 x i8]
@_dv85 = external global [5 x i8]
@_dv86 = external global [2 x i8]
@_dv87 = external global [2 x i8]
@_dv88 = external global [10 x i8]
@_dv89 = external global [10 x i8]
@_dv90 = external global [4 x i8]
@_dv91 = external global [8 x i8]
@_dv92 = external global [10 x i8]
@_dv93 = external global [5 x i8]
@_dv94 = external global [2 x i8]
@_dv95 = external global [2 x i8]
@_dv96 = external global [14 x i8]
@_dv97 = external global [8 x i8]
@_dv98 = external global [10 x i8]
@_dv99 = external global [6 x i8]
@_dv64 = external global [7 x i8]
@_dv53 = external global [6 x i8]
@_dv18 = external global [4 x i8]
@_dv4 = external global [3 x i8]
@_dv103 = external global [3 x i8]
@_dv104 = external global [10 x i8]
@_dv105 = external global [4 x i8]
@_dv106 = external global [4 x i8]
@_dv107 = external global [5 x i8]
@_dv108 = external global [2 x i8]
@_dv109 = external global [6 x i8]
@_dv110 = external global [16 x i8]
@_dv111 = external global [3 x i8]
@_dv112 = external global [5 x i8]
@_dv113 = external global [11 x i8]
@_dv114 = external global [4 x i8]
@_dv115 = external global [10 x i8]
@_dv116 = external global [4 x i8]
@_dv117 = external global [5 x i8]
@_dv118 = external global [2 x i8]
@_dv119 = external global [6 x i8]
@_dv120 = external global [4 x i8]
@_dv121 = external global [9 x i8]
@_dv122 = external global [4 x i8]
@_dv123 = external global [5 x i8]
@_dv124 = external global [2 x i8]
@_dv125 = external global [6 x i8]
@_dv126 = external global [4 x i8]
@_dv127 = external global [8 x i8]
@_dv128 = external global [4 x i8]
@_dv129 = external global [5 x i8]
@_dv130 = external global [2 x i8]
@_dv131 = external global [6 x i8]
@res = external global %struct__Z5DNode*
@_dv0 = external global [4 x i8]
@_dv1 = external global [10 x i8]
@_dv2 = external global [3 x i8]
@_dv3 = external global [2 x i8]
@arg-count = external global i32
@arglist = external global %"struct__Z7va$2dlist"
@begin = external global i32
@block = external global %struct__Z5DNode*
@blockstart = external global %struct__Z5DNode*
@cn = external global %struct__Z5DNode*
@current-firstnode = external global %struct__Z5DNode*
@current-lastnode = external global %struct__Z5DNode*
@holder = external global %struct__Z5DNode*
@lastnodes = external global [256 x %struct__Z5DNode*]
@myargcount = external global i32
@no-down = external global i32
@nullp = external global %struct__Z5DNode*
@processed-list = external global i32
@repl-next = external global i32
@temp-node = external global %struct__Z5DNode*
@tempform = external global %struct__Z5DNode*
@upvs = external global [256 x %struct__Z5DNode*]
@upvs-count = external global i32
@va-dnode = external global %struct__Z5DNode*
@stderr = external global %struct__Z4file*
@stdin = external global %struct__Z4file*
@stdout = external global %struct__Z4file*
@seed = external global i32
@_dale_internal_var_0 = external global %struct__Z5DNode*
@firstnode = external global %struct__Z5DNode*
@newnode = external global %struct__Z5DNode*
@trailing = external global %struct__Z5DNode*
@_dv132 = external global [7 x i8]
@_dv133 = external global [3 x i8]
@_dv134 = external global [3 x i8]
@begin-column-node = external global %struct__Z5DNode*
@begin-column-str = external global [25 x i8]
@begin-line-node = external global %struct__Z5DNode*
@begin-line-str = external global [25 x i8]
@cond-str = external global [1024 x i8]
@cond-str-node = external global %struct__Z5DNode*
@filename-node = external global %struct__Z5DNode*
@filename-str = external global [1024 x i8]
@last-charp = external global i8*
@condition = external global %struct__Z5DNode*
@c = external global i32
@m = external global x86_fp80
@a = external global %struct__Z3_vp*
@b = external global i32
@__s = external global i8*
@__environ = external global i8**
@optarg = external global i8*
@opterr = external global i32
@optind = external global i32
@optopt = external global i32
@_dv486 = external global [36 x i8]
@_dv487 = external global [6 x i8]
@_dv488 = external global [33 x i8]
@_dv489 = external global [30 x i8]
@_dv490 = external global [25 x i8]
@_dv491 = external global [22 x i8]
@_dv492 = external global [32 x i8]
@type = external global %struct__Z5DNode*
@concept = external global %struct__Z5DNode*
@first = external global %struct__Z5DNode*
@_dv493 = external global [6 x i8]
@_dv494 = external global [37 x i8]
@i = external global i32
@_dv495 = external global [34 x i8]
@_dv496 = external global [17 x i8]
@_dale_internal_var_26 = external global %struct__Z5DNode*
@_dale_internal_var_27 = external global %struct__Z5DNode*
@macnode = external global %struct__Z5DNode*
@retnode = external global %struct__Z5DNode*
@err = external global i32
@macbuf = external global [255 x i8]
@arg-array = external global %struct__Z5DNode**
@arg-array-original = external global %struct__Z5DNode**
@arg-count-original = external global i32
@macro-name = external global %struct__Z5DNode*
@myvl = external global %struct__Z5DNode*
@pre-arg-count = external global i32
@type-concept-list = external global [64 x %"struct__ZN3std8concepts12concept$2dnodeE"*]
@varargs-list = external global %struct__Z5DNode*
@_dv223 = external global [7 x i8]
@typename = external global [250 x i8]
@T1 = external global %struct__Z5DNode*
@T2 = external global %struct__Z5DNode*
@T3 = external global %struct__Z5DNode*
@_dale_internal_var_1 = external global %struct__Z5DNode*
@_dale_internal_var_2 = external global %struct__Z5DNode*
@_dale_internal_var_3 = external global %struct__Z5DNode*
@_dv159 = external global [7 x i8]
@_dv160 = external global [7 x i8]
@etypename = external global [250 x i8]
@typenode = external global %struct__Z5DNode*
@_dv619 = external global [25 x i8]
@_dv620 = external global [3 x i8]
@_dv621 = external global [5 x i8]
@_dv624 = external global [25 x i8]
@_dv625 = external global [2 x i8]
@_dv626 = external global [5 x i8]
@_dv629 = external global [25 x i8]
@_dv630 = external global [3 x i8]
@_dv631 = external global [5 x i8]
@_dv604 = external global [25 x i8]
@_dv605 = external global [2 x i8]
@_dv606 = external global [5 x i8]
@_dv609 = external global [25 x i8]
@_dv610 = external global [3 x i8]
@_dv611 = external global [5 x i8]
@_dv614 = external global [25 x i8]
@_dv615 = external global [2 x i8]
@_dv616 = external global [5 x i8]
@_dv602 = external global [5 x i8]
@_dv603 = external global [2 x i8]
@_dv607 = external global [5 x i8]
@_dv608 = external global [3 x i8]
@_dv612 = external global [5 x i8]
@_dv613 = external global [2 x i8]
@_dv617 = external global [5 x i8]
@_dv618 = external global [3 x i8]
@_dv622 = external global [5 x i8]
@_dv623 = external global [2 x i8]
@_dv627 = external global [5 x i8]
@_dv628 = external global [3 x i8]
@_dv632 = external global [3 x i8]
@"n!=" = external global %struct__Z5DNode*
@"n<" = external global %struct__Z5DNode*
@"n<=" = external global %struct__Z5DNode*
@"n=" = external global %struct__Z5DNode*
@"n>" = external global %struct__Z5DNode*
@"n>=" = external global %struct__Z5DNode*
@_dale_internal_var_38 = external global %struct__Z5DNode*
@Tc = external global %struct__Z5DNode*
@_dv135 = internal constant %struct__Z5DNode { i32 1, i8* null, %struct__Z5DNode* @_dv138, %struct__Z5DNode* null, i32 0, i32 0, i32 0, i32 0, i32 185, i32 9, i32 185, i32 22, i8* null }
@_dv136 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([4 x i8]* @_dv137, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* null, i32 4, i32 34, i32 4, i32 37, i32 4, i32 1, i32 4, i32 37, i8* null }
@_dv137 = internal constant [4 x i8] c"int\00"
@_dv138 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([3 x i8]* @_dv139, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* @_dv136, i32 185, i32 13, i32 185, i32 15, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv139 = internal constant [3 x i8] c"do\00"
@_dv140 = internal constant %struct__Z5DNode { i32 1, i8* null, %struct__Z5DNode* @_dv143, %struct__Z5DNode* null, i32 0, i32 0, i32 0, i32 0, i32 198, i32 9, i32 198, i32 20, i8* null }
@_dv141 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([5 x i8]* @_dv142, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* null, i32 198, i32 16, i32 198, i32 20, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv142 = internal constant [5 x i8] c"size\00"
@_dv143 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([3 x i8]* @_dv139, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* @_dv141, i32 198, i32 13, i32 198, i32 15, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv144 = internal constant %struct__Z5DNode { i32 1, i8* null, %struct__Z5DNode* @_dv147, %struct__Z5DNode* null, i32 0, i32 0, i32 0, i32 0, i32 211, i32 9, i32 211, i32 23, i8* null }
@_dv145 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([8 x i8]* @_dv146, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* null, i32 211, i32 16, i32 211, i32 23, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv146 = internal constant [8 x i8] c"ptrdiff\00"
@_dv147 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([3 x i8]* @_dv139, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* @_dv145, i32 211, i32 13, i32 211, i32 15, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv148 = internal constant %struct__Z5DNode { i32 1, i8* null, %struct__Z5DNode* @_dv150, %struct__Z5DNode* null, i32 0, i32 0, i32 0, i32 0, i32 413, i32 9, i32 413, i32 22, i8* null }
@_dv149 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([4 x i8]* @_dv137, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* null, i32 4, i32 34, i32 4, i32 37, i32 4, i32 1, i32 4, i32 37, i8* null }
@_dv150 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([3 x i8]* @_dv139, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* @_dv149, i32 413, i32 13, i32 413, i32 15, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv151 = internal constant %struct__Z5DNode { i32 1, i8* null, %struct__Z5DNode* @_dv153, %struct__Z5DNode* null, i32 0, i32 0, i32 0, i32 0, i32 426, i32 9, i32 426, i32 20, i8* null }
@_dv152 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([5 x i8]* @_dv142, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* null, i32 426, i32 16, i32 426, i32 20, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv153 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([3 x i8]* @_dv139, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* @_dv152, i32 426, i32 13, i32 426, i32 15, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv154 = internal constant %struct__Z5DNode { i32 1, i8* null, %struct__Z5DNode* @_dv156, %struct__Z5DNode* null, i32 0, i32 0, i32 0, i32 0, i32 647, i32 9, i32 647, i32 22, i8* null }
@_dv155 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([4 x i8]* @_dv137, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* null, i32 4, i32 34, i32 4, i32 37, i32 4, i32 1, i32 4, i32 37, i8* null }
@_dv156 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([3 x i8]* @_dv139, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* @_dv155, i32 647, i32 13, i32 647, i32 15, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv157 = internal constant %struct__Z5DNode { i32 1, i8* null, %struct__Z5DNode* @_dv161, %struct__Z5DNode* null, i32 0, i32 0, i32 0, i32 0, i32 651, i32 9, i32 651, i32 20, i8* null }
@_dv158 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([5 x i8]* @_dv142, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* null, i32 651, i32 16, i32 651, i32 20, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv161 = internal constant %struct__Z5DNode { i32 0, i8* getelementptr inbounds ([3 x i8]* @_dv139, i32 0, i32 0), %struct__Z5DNode* null, %struct__Z5DNode* @_dv158, i32 651, i32 13, i32 651, i32 15, i32 59, i32 3, i32 876, i32 5, i8* null }
@_dv162 = internal constant [4 x i8] c"%d\0A\00"
@_dv163 = internal constant [16 x i8] c"Three elements\0A\00"
@_dv164 = internal constant [25 x i8] c"First and third element\0A\00"
@_dv165 = internal constant [15 x i8] c"Third element\0A\00"
@_dv166 = internal constant [13 x i8] c"No elements\0A\00"

declare void @llvm.va_start(i8*) nounwind

declare void @llvm.va_end(i8*) nounwind

declare i1 @"_Z2$21$3dii"(i32, i32)

declare i1 @"_Z2$21$3dcc"(i8, i8)

declare i1 @"_Z2$21$3dyy"(i8, i8)

declare i1 @"_Z2$21$3dnn"(i16, i16)

declare i1 @"_Z2$21$3dpp"(i32, i32)

declare i1 @"_Z2$21$3drr"(i64, i64)

declare i1 @"_Z2$21$3djj"(i32, i32)

declare i1 @"_Z2$21$3dll"(i32, i32)

declare i1 @"_Z2$21$3dxx"(i32, i32)

declare i1 @"_Z2$21$3dww"(i32, i32)

declare i1 @"_Z2$21$3dmm"(i8, i8)

declare i1 @"_Z2$21$3doo"(i16, i16)

declare i1 @"_Z2$21$3dqq"(i32, i32)

declare i1 @"_Z2$21$3dss"(i64, i64)

declare i1 @"_Z2$21$3dff"(float, float)

declare i1 @"_Z2$21$3ddd"(double, double)

declare i1 @"_Z2$21$3daa"(x86_fp80, x86_fp80)

declare i32 @"_Z1$26ii"(i32, i32)

declare i8 @"_Z1$26cc"(i8, i8)

declare i8 @"_Z1$26yy"(i8, i8)

declare i16 @"_Z1$26nn"(i16, i16)

declare i32 @"_Z1$26pp"(i32, i32)

declare i64 @"_Z1$26rr"(i64, i64)

declare i32 @"_Z1$26jj"(i32, i32)

declare i32 @"_Z1$26ll"(i32, i32)

declare i32 @"_Z1$26xx"(i32, i32)

declare i32 @"_Z1$26ww"(i32, i32)

declare i8 @"_Z1$26mm"(i8, i8)

declare i16 @"_Z1$26oo"(i16, i16)

declare i32 @"_Z1$26qq"(i32, i32)

declare i64 @"_Z1$26ss"(i64, i64)

declare i32 @"_Z1$2aii"(i32, i32)

declare i8 @"_Z1$2acc"(i8, i8)

declare i8 @"_Z1$2ayy"(i8, i8)

declare i16 @"_Z1$2ann"(i16, i16)

declare i32 @"_Z1$2app"(i32, i32)

declare i64 @"_Z1$2arr"(i64, i64)

declare i32 @"_Z1$2ajj"(i32, i32)

declare i32 @"_Z1$2all"(i32, i32)

declare i32 @"_Z1$2axx"(i32, i32)

declare i32 @"_Z1$2aww"(i32, i32)

declare i8 @"_Z1$2amm"(i8, i8)

declare i16 @"_Z1$2aoo"(i16, i16)

declare i32 @"_Z1$2aqq"(i32, i32)

declare i64 @"_Z1$2ass"(i64, i64)

declare float @"_Z1$2aff"(float, float)

declare double @"_Z1$2add"(double, double)

declare x86_fp80 @"_Z1$2aaa"(x86_fp80, x86_fp80)

declare i32 @"_Z1$2bii"(i32, i32)

declare i8 @"_Z1$2bcc"(i8, i8)

declare i8 @"_Z1$2byy"(i8, i8)

declare i16 @"_Z1$2bnn"(i16, i16)

declare i32 @"_Z1$2bpp"(i32, i32)

declare i64 @"_Z1$2brr"(i64, i64)

declare i32 @"_Z1$2bjj"(i32, i32)

declare i32 @"_Z1$2bll"(i32, i32)

declare i32 @"_Z1$2bxx"(i32, i32)

declare i32 @"_Z1$2bww"(i32, i32)

declare i8 @"_Z1$2bmm"(i8, i8)

declare i16 @"_Z1$2boo"(i16, i16)

declare i32 @"_Z1$2bqq"(i32, i32)

declare i64 @"_Z1$2bss"(i64, i64)

declare float @"_Z1$2bff"(float, float)

declare double @"_Z1$2bdd"(double, double)

declare x86_fp80 @"_Z1$2baa"(x86_fp80, x86_fp80)

declare i32 @"_Z1$2dii"(i32, i32)

declare i8 @"_Z1$2dcc"(i8, i8)

declare i8 @"_Z1$2dyy"(i8, i8)

declare i16 @"_Z1$2dnn"(i16, i16)

declare i32 @"_Z1$2dpp"(i32, i32)

declare i64 @"_Z1$2drr"(i64, i64)

declare i32 @"_Z1$2djj"(i32, i32)

declare i32 @"_Z1$2dll"(i32, i32)

declare i32 @"_Z1$2dxx"(i32, i32)

declare i32 @"_Z1$2dww"(i32, i32)

declare i8 @"_Z1$2dmm"(i8, i8)

declare i16 @"_Z1$2doo"(i16, i16)

declare i32 @"_Z1$2dqq"(i32, i32)

declare i64 @"_Z1$2dss"(i64, i64)

declare float @"_Z1$2dff"(float, float)

declare double @"_Z1$2ddd"(double, double)

declare x86_fp80 @"_Z1$2daa"(x86_fp80, x86_fp80)

declare i32 @"_Z1$2fii"(i32, i32)

declare i8 @"_Z1$2fcc"(i8, i8)

declare i8 @"_Z1$2fyy"(i8, i8)

declare i16 @"_Z1$2fnn"(i16, i16)

declare i32 @"_Z1$2fpp"(i32, i32)

declare i64 @"_Z1$2frr"(i64, i64)

declare i32 @"_Z1$2fjj"(i32, i32)

declare i32 @"_Z1$2fll"(i32, i32)

declare i32 @"_Z1$2fxx"(i32, i32)

declare i32 @"_Z1$2fww"(i32, i32)

declare i8 @"_Z1$2fmm"(i8, i8)

declare i16 @"_Z1$2foo"(i16, i16)

declare i32 @"_Z1$2fqq"(i32, i32)

declare i64 @"_Z1$2fss"(i64, i64)

declare float @"_Z1$2fff"(float, float)

declare double @"_Z1$2fdd"(double, double)

declare x86_fp80 @"_Z1$2faa"(x86_fp80, x86_fp80)

declare i1 @"_Z1$3cii"(i32, i32)

declare i1 @"_Z1$3ccc"(i8, i8)

declare i1 @"_Z1$3cyy"(i8, i8)

declare i1 @"_Z1$3cnn"(i16, i16)

declare i1 @"_Z1$3cpp"(i32, i32)

declare i1 @"_Z1$3crr"(i64, i64)

declare i1 @"_Z1$3cjj"(i32, i32)

declare i1 @"_Z1$3cll"(i32, i32)

declare i1 @"_Z1$3cxx"(i32, i32)

declare i1 @"_Z1$3cww"(i32, i32)

declare i1 @"_Z1$3cmm"(i8, i8)

declare i1 @"_Z1$3coo"(i16, i16)

declare i1 @"_Z1$3cqq"(i32, i32)

declare i1 @"_Z1$3css"(i64, i64)

declare i1 @"_Z1$3cff"(float, float)

declare i1 @"_Z1$3cdd"(double, double)

declare i1 @"_Z1$3caa"(x86_fp80, x86_fp80)

declare i32 @"_Z2$3c$3cii"(i32, i32)

declare i8 @"_Z2$3c$3cci"(i8, i32)

declare i8 @"_Z2$3c$3cyi"(i8, i32)

declare i16 @"_Z2$3c$3cni"(i16, i32)

declare i32 @"_Z2$3c$3cpi"(i32, i32)

declare i64 @"_Z2$3c$3cri"(i64, i32)

declare i32 @"_Z2$3c$3cji"(i32, i32)

declare i32 @"_Z2$3c$3cli"(i32, i32)

declare i32 @"_Z2$3c$3cxi"(i32, i32)

declare i32 @"_Z2$3c$3cwi"(i32, i32)

declare i8 @"_Z2$3c$3cmi"(i8, i32)

declare i16 @"_Z2$3c$3coi"(i16, i32)

declare i32 @"_Z2$3c$3cqi"(i32, i32)

declare i64 @"_Z2$3c$3csi"(i64, i32)

declare i1 @"_Z2$3c$3dii"(i32, i32)

declare i1 @"_Z2$3c$3dcc"(i8, i8)

declare i1 @"_Z2$3c$3dyy"(i8, i8)

declare i1 @"_Z2$3c$3dnn"(i16, i16)

declare i1 @"_Z2$3c$3dpp"(i32, i32)

declare i1 @"_Z2$3c$3drr"(i64, i64)

declare i1 @"_Z2$3c$3djj"(i32, i32)

declare i1 @"_Z2$3c$3dll"(i32, i32)

declare i1 @"_Z2$3c$3dxx"(i32, i32)

declare i1 @"_Z2$3c$3dww"(i32, i32)

declare i1 @"_Z2$3c$3dmm"(i8, i8)

declare i1 @"_Z2$3c$3doo"(i16, i16)

declare i1 @"_Z2$3c$3dqq"(i32, i32)

declare i1 @"_Z2$3c$3dss"(i64, i64)

declare i1 @"_Z2$3c$3dff"(float, float)

declare i1 @"_Z2$3c$3ddd"(double, double)

declare i1 @"_Z2$3c$3daa"(x86_fp80, x86_fp80)

declare i1 @"_Z1$3dii"(i32, i32)

declare i1 @"_Z1$3dcc"(i8, i8)

declare i1 @"_Z1$3dyy"(i8, i8)

declare i1 @"_Z1$3dnn"(i16, i16)

declare i1 @"_Z1$3dpp"(i32, i32)

declare i1 @"_Z1$3drr"(i64, i64)

declare i1 @"_Z1$3djj"(i32, i32)

declare i1 @"_Z1$3dll"(i32, i32)

declare i1 @"_Z1$3dxx"(i32, i32)

declare i1 @"_Z1$3dww"(i32, i32)

declare i1 @"_Z1$3dmm"(i8, i8)

declare i1 @"_Z1$3doo"(i16, i16)

declare i1 @"_Z1$3dqq"(i32, i32)

declare i1 @"_Z1$3dss"(i64, i64)

declare i1 @"_Z1$3dff"(float, float)

declare i1 @"_Z1$3ddd"(double, double)

declare i1 @"_Z1$3daa"(x86_fp80, x86_fp80)

declare i1 @"_Z1$3eii"(i32, i32)

declare i1 @"_Z1$3ecc"(i8, i8)

declare i1 @"_Z1$3eyy"(i8, i8)

declare i1 @"_Z1$3enn"(i16, i16)

declare i1 @"_Z1$3epp"(i32, i32)

declare i1 @"_Z1$3err"(i64, i64)

declare i1 @"_Z1$3ejj"(i32, i32)

declare i1 @"_Z1$3ell"(i32, i32)

declare i1 @"_Z1$3exx"(i32, i32)

declare i1 @"_Z1$3eww"(i32, i32)

declare i1 @"_Z1$3emm"(i8, i8)

declare i1 @"_Z1$3eoo"(i16, i16)

declare i1 @"_Z1$3eqq"(i32, i32)

declare i1 @"_Z1$3ess"(i64, i64)

declare i1 @"_Z1$3eff"(float, float)

declare i1 @"_Z1$3edd"(double, double)

declare i1 @"_Z1$3eaa"(x86_fp80, x86_fp80)

declare i1 @"_Z2$3e$3dii"(i32, i32)

declare i1 @"_Z2$3e$3dcc"(i8, i8)

declare i1 @"_Z2$3e$3dyy"(i8, i8)

declare i1 @"_Z2$3e$3dnn"(i16, i16)

declare i1 @"_Z2$3e$3dpp"(i32, i32)

declare i1 @"_Z2$3e$3drr"(i64, i64)

declare i1 @"_Z2$3e$3djj"(i32, i32)

declare i1 @"_Z2$3e$3dll"(i32, i32)

declare i1 @"_Z2$3e$3dxx"(i32, i32)

declare i1 @"_Z2$3e$3dww"(i32, i32)

declare i1 @"_Z2$3e$3dmm"(i8, i8)

declare i1 @"_Z2$3e$3doo"(i16, i16)

declare i1 @"_Z2$3e$3dqq"(i32, i32)

declare i1 @"_Z2$3e$3dss"(i64, i64)

declare i1 @"_Z2$3e$3dff"(float, float)

declare i1 @"_Z2$3e$3ddd"(double, double)

declare i1 @"_Z2$3e$3daa"(x86_fp80, x86_fp80)

declare i32 @"_Z2$3e$3eii"(i32, i32)

declare i8 @"_Z2$3e$3eci"(i8, i32)

declare i8 @"_Z2$3e$3eyi"(i8, i32)

declare i16 @"_Z2$3e$3eni"(i16, i32)

declare i32 @"_Z2$3e$3epi"(i32, i32)

declare i64 @"_Z2$3e$3eri"(i64, i32)

declare i32 @"_Z2$3e$3eji"(i32, i32)

declare i32 @"_Z2$3e$3eli"(i32, i32)

declare i32 @"_Z2$3e$3exi"(i32, i32)

declare i32 @"_Z2$3e$3ewi"(i32, i32)

declare i8 @"_Z2$3e$3emi"(i8, i32)

declare i16 @"_Z2$3e$3eoi"(i16, i32)

declare i32 @"_Z2$3e$3eqi"(i32, i32)

declare i64 @"_Z2$3e$3esi"(i64, i32)

declare i32 @"_Z1$5eii"(i32, i32)

declare i8 @"_Z1$5ecc"(i8, i8)

declare i8 @"_Z1$5eyy"(i8, i8)

declare i16 @"_Z1$5enn"(i16, i16)

declare i32 @"_Z1$5epp"(i32, i32)

declare i64 @"_Z1$5err"(i64, i64)

declare i32 @"_Z1$5ejj"(i32, i32)

declare i32 @"_Z1$5ell"(i32, i32)

declare i32 @"_Z1$5exx"(i32, i32)

declare i32 @"_Z1$5eww"(i32, i32)

declare i8 @"_Z1$5emm"(i8, i8)

declare i16 @"_Z1$5eoo"(i16, i16)

declare i32 @"_Z1$5eqq"(i32, i32)

declare i64 @"_Z1$5ess"(i64, i64)

declare i32 @"_Z9arg$2dcountPZ8MContext"(%struct__Z8MContext*)

declare void @free(%struct__Z3_vp*)

declare %struct__Z3_vp* @malloc(i32)

declare %struct__Z3_vp* @memcpy(%struct__Z3_vp*, %struct__Z3_vp*, i32)

declare i1 @_Z3notb(i1)

declare void @"_Z9pool$2dfreePZ8MContext"(%struct__Z8MContext*)

declare void @"_Z10pool$2dfree_PZ8PoolNode"(%struct__Z8PoolNode*)

declare %struct__Z3_vp* @"_Z11pool$2dmallocPZ8MContextw"(%struct__Z8MContext*, i32)

declare i32 @"_Z1$7cii"(i32, i32)

declare i8 @"_Z1$7ccc"(i8, i8)

declare i8 @"_Z1$7cyy"(i8, i8)

declare i16 @"_Z1$7cnn"(i16, i16)

declare i32 @"_Z1$7cpp"(i32, i32)

declare i64 @"_Z1$7crr"(i64, i64)

declare i32 @"_Z1$7cjj"(i32, i32)

declare i32 @"_Z1$7cll"(i32, i32)

declare i32 @"_Z1$7cxx"(i32, i32)

declare i32 @"_Z1$7cww"(i32, i32)

declare i8 @"_Z1$7cmm"(i8, i8)

declare i16 @"_Z1$7coo"(i16, i16)

declare i32 @"_Z1$7cqq"(i32, i32)

declare i64 @"_Z1$7css"(i64, i64)

declare i32 @"_Z1$7ej"(i32)

declare i32 @"_Z1$7el"(i32)

declare i32 @"_Z1$7ex"(i32)

declare i32 @"_Z1$7ew"(i32)

declare i8 @"_Z1$7em"(i8)

declare i16 @"_Z1$7eo"(i16)

declare i32 @"_Z1$7eq"(i32)

declare i64 @"_Z1$7es"(i64)

declare i32 @fprintf(%struct__Z4file*, i8*, ...)

declare %struct__Z3_vp* @memset(%struct__Z3_vp*, i32, i32)

declare i32 @printf(i8*, ...)

declare i32 @sprintf(i8*, i8*, ...)

declare i8* @strchr(i8*, i32)

declare i32 @strcmp(i8*, i8*)

declare i32 @strlen(i8*)

declare i32 @strncmp(i8*, i8*, i32)

declare i8* @strncpy(i8*, i8*, i32)

declare i32 @arity(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @codomain(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @exists-fn(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @exists-macro(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @exists-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @exists-variable(%struct__Z8MContext*, %struct__Z5DNode*)

declare i32 @fn-by-args-count(%struct__Z8MContext*, %struct__Z5DNode*, i8*)

declare i8* @fn-by-args-name(%struct__Z8MContext*, %struct__Z5DNode*, i32)

declare i1 @has-errors(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @input-type(%struct__Z8MContext*, %struct__Z5DNode*, i32)

declare i1 @is-char-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @is-const(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @is-floating-point-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @is-integer-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @is-pointer-to-type(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare i1 @is-pointer-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @is-signed-integer-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @is-unsigned-integer-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @must-init(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @pointee-type(%struct__Z8MContext*, %struct__Z5DNode*)

declare i8* @printf-length(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @register-type(%struct__Z8MContext*, i8*, i8*)

declare void @report-error(%struct__Z8MContext*, %struct__Z5DNode*, i8*)

declare i32 @struct-member-count(%struct__Z8MContext*, %struct__Z5DNode*)

declare i8* @struct-member-name(%struct__Z8MContext*, %struct__Z5DNode*, i32)

declare %struct__Z5DNode* @struct-member-type(%struct__Z8MContext*, %struct__Z5DNode*, i32)

declare %struct__Z5DNode* @type-of(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @type-to-display-string(%struct__Z8MContext*, %struct__Z5DNode*, i8*)

declare i1 @type-to-string(%struct__Z8MContext*, %struct__Z5DNode*, i8*)

declare i1 @types-equal(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_ZN3std6macros3bqqEPZ8MContextPZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @"_ZN3std6macros10bqq$2dhelperEPZ5DNodePZ8MContexti"(%struct__Z5DNode*, %struct__Z8MContext*, i32)

declare %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext*, i1, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std6macros9copy$2dqstrEPZ8MContextPZ5DNodeb"(%struct__Z8MContext*, %struct__Z5DNode*, i1)

declare void @"_ZN3std6macros7copy$2dtoEPZ5DNodePZ5DNode"(%struct__Z5DNode*, %struct__Z5DNode*)

declare i1 @"_ZN3std6macros12gensym$2dlabelEPcPc"(i8*, i8*)

declare i1 @"_ZN3std6macros10gensym$2dvarEPc"(i8*)

declare %struct__Z5DNode* @"_ZN3std6macros13get$2dlast$2dnodeEPZ5DNode"(%struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std6macros17get$2dvarargs$2darrayEPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_ZN3std6macros16get$2dvarargs$2dlistEPZ8MContext"(%struct__Z8MContext*)

declare i1 @"_ZN3std6macros15is$2dgensym$2dlabelEPZ5DNodePc"(%struct__Z5DNode*, i8*)

declare i8* @_ZN3std6macros4itoaEi(i32)

declare %struct__Z5DNode* @"_ZN3std6macros10link$2dnodesEiz"(i32, ...)

declare %struct__Z5DNode* @"_ZN3std6macros16link$2dnodes$2darrayEiPPZ5DNode"(i32, %struct__Z5DNode**)

declare %struct__Z5DNode* @"_ZN3std6macros15link$2dnodes$2dlistEPZ8MContextiz"(%struct__Z8MContext*, i32, ...)

declare %struct__Z5DNode* @"_ZN3std6macros18link$2dnodes$2dlist$2dwpEPZ8MContextiiiiiiiiiz"(%struct__Z8MContext*, i32, i32, i32, i32, i32, i32, i32, i32, i32, ...)

declare i32 @"_ZN3std6macros10list$2dcountEPZ5DNode"(%struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std6macros22make$2dgensym$2dlabel$2dnodeEPZ8MContextPc"(%struct__Z8MContext*, i8*)

declare %struct__Z5DNode* @"_ZN3std6macros22make$2dgensym$2dlabel$2dnodeEPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_ZN3std6macros20make$2dgensym$2dvar$2dnodeEPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_ZN3std6macros9make$2dnodeEPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_ZN3std6macros15make$2dnode$2dmakerEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_ZN3std6macros4mnfvEPZ8MContextPc(%struct__Z8MContext*, i8*)

declare %struct__Z5DNode* @_ZN3std6macros4mnfvEPZ8MContexti(%struct__Z8MContext*, i32)

declare %struct__Z5DNode* @_ZN3std6macros4mnfvEPZ8MContextf(%struct__Z8MContext*, float)

declare %struct__Z5DNode* @_ZN3std6macros4mnfvEPZ8MContextd(%struct__Z8MContext*, double)

declare %struct__Z5DNode* @_ZN3std6macros4mnfvEPZ8MContexta(%struct__Z8MContext*, x86_fp80)

declare %struct__Z5DNode* @"_ZN3std6macros7mnfv$2dwpEPZ8MContextPciiiiiiii"(%struct__Z8MContext*, i8*, i32, i32, i32, i32, i32, i32, i32, i32)

declare %struct__Z5DNode* @"_ZN3std6macros12pool$2dmalloc$27EPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare i1 @_ZN3std6macros5printEPZ5DNode(%struct__Z5DNode*)

declare i1 @_ZN3std6macros6sprintEPcPZ5DNode(i8*, %struct__Z5DNode*)

declare i32 @"_ZN3std6macros21valist$2dto$2ddnode$2darrayEPPviPPZ5DNode"(%struct__Z3_vp**, i32, %struct__Z5DNode**)

declare i1 @"_ZN3std6macros10walk$2dnodesEPZ5DNodePZ8MContextPvPFiPZ5DNodePZ8MContextPvE"(%struct__Z5DNode*, %struct__Z8MContext*, %struct__Z3_vp*, i32 (%struct__Z5DNode*, %struct__Z8MContext*, %struct__Z3_vp*)*)

declare %struct__Z5DNode* @"_Z3$40$3a$27PZ8MContextPZ5DNodePZ5DNodez"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @_Z3andPZ8MContextPZ5DNodePZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z4decfPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z4decvPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z3forPZ8MContextPZ5DNodePZ5DNodePZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @"_Z5free$27PZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z4incfPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z4incvPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z3letPZ8MContextPZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @"_Z14long$2dlong$2dtypePZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z9long$2dtypePZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z19make$2dmacro$2dconstantPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z7malloc$27PZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z4mforPZ8MContextPZ5DNodePZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @"_Z5no$2dopPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z2orPZ8MContextPZ5DNodePZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z3p$3c$3dPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z3p$3e$3dPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z15ulong$2dlong$2dtypePZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z10ulong$2dtypePZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z5whilePZ8MContextPZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @_ZN3std6macros2qqEPZ8MContextPZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, ...)

declare i1 @"_Z2$21$3dZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare i1 @"_Z2$21$3dZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare %"struct__Z11buffer$2dmode" @"_Z1$26Z11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare %struct__Z11fseekorigin @"_Z1$26Z11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare %"struct__Z11buffer$2dmode" @"_Z1$2aZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare %struct__Z11fseekorigin @"_Z1$2aZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare %"struct__Z11buffer$2dmode" @"_Z1$2bZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare %struct__Z11fseekorigin @"_Z1$2bZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare %"struct__Z11buffer$2dmode" @"_Z1$2dZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare %struct__Z11fseekorigin @"_Z1$2dZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare %"struct__Z11buffer$2dmode" @"_Z1$2fZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare %struct__Z11fseekorigin @"_Z1$2fZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare i1 @"_Z1$3cZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare i1 @"_Z1$3cZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare %"struct__Z11buffer$2dmode" @"_Z2$3c$3cZ11buffer-modei"(%"struct__Z11buffer$2dmode", i32)

declare %struct__Z11fseekorigin @"_Z2$3c$3cZ11fseekorigini"(%struct__Z11fseekorigin, i32)

declare i1 @"_Z2$3c$3dZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare i1 @"_Z2$3c$3dZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare i1 @"_Z1$3dZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare i1 @"_Z1$3dZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare i1 @"_Z1$3eZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare i1 @"_Z1$3eZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare i1 @"_Z2$3e$3dZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare i1 @"_Z2$3e$3dZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare %"struct__Z11buffer$2dmode" @"_Z2$3e$3eZ11buffer-modei"(%"struct__Z11buffer$2dmode", i32)

declare %struct__Z11fseekorigin @"_Z2$3e$3eZ11fseekorigini"(%struct__Z11fseekorigin, i32)

declare %struct__Z5DNode* @"_Z12EXIT$2dFAILUREPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z12EXIT$2dSUCCESSPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z12FILENAME$2dMAXPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z9FOPEN$2dMAXPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z8L$2dtmpnamPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z7MFPOS_TPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z8RAND$2dMAXPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_Z7TMP$2dMAXPZ8MContext"(%struct__Z8MContext*)

declare %"struct__Z11buffer$2dmode" @"_Z1$5eZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare %struct__Z11fseekorigin @"_Z1$5eZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare void @clearerr(%struct__Z4file*)

declare i32 @fclose(%struct__Z4file*)

declare %struct__Z4file* @fdopen(i32, i8*)

declare i32 @feof(%struct__Z4file*)

declare i32 @ferror(%struct__Z4file*)

declare i32 @fflush(%struct__Z4file*)

declare i32 @fgetc(%struct__Z4file*)

declare i32 @fgetpos(%struct__Z4file*, %struct__Z4fpos*)

declare i8* @fgets(i8*, i32, %struct__Z4file*)

declare %struct__Z4file* @fopen(i8*, i8*)

declare i32 @fputc(i32, %struct__Z4file*)

declare i32 @fputs(i8*, %struct__Z4file*)

declare i32 @fread(%struct__Z3_vp*, i32, i32, %struct__Z4file*)

declare %struct__Z4file* @freopen(i8*, i8*, %struct__Z4file*)

declare i32 @fseek(%struct__Z4file*, i32, %struct__Z11fseekorigin)

declare i32 @fsetpos(%struct__Z4file*, %struct__Z4fpos*)

declare i32 @ftell(%struct__Z4file*)

declare i32 @fwrite(%struct__Z3_vp*, i32, i32, %struct__Z4file*)

declare i32 @getc(%struct__Z4file*)

declare i32 @getchar(%struct__Z4file*)

declare i8* @gets(i8*)

declare i32 @"_Z13init$2dchannels"()

declare void @perror(i8*)

declare i32 @putc(i32, %struct__Z4file*)

declare i32 @putchar(i32)

declare i32 @puts(i8*)

declare i32 @remove(i8*)

declare i32 @rename(i8*, i8*)

declare void @rewind(%struct__Z4file*)

declare void @setbuf(%struct__Z4file*, i8*)

declare i32 @setvbuf(%struct__Z4file*, i8*, %"struct__Z11buffer$2dmode", i32)

declare %struct__Z4file* @tmpfile()

declare i8* @tmpnam(i8*)

declare i32 @ungetc(i32, %struct__Z4file*)

declare i32 @vfprintf(%struct__Z4file*, i8*, %"struct__Z7va$2dlist")

declare i32 @vprintf(i8*, %"struct__Z7va$2dlist")

declare i32 @vsprintf(%struct__Z4file*, i8*, %"struct__Z7va$2dlist")

declare %"struct__Z11buffer$2dmode" @"_Z1$7cZ11buffer-modeZ11buffer-mode"(%"struct__Z11buffer$2dmode", %"struct__Z11buffer$2dmode")

declare %struct__Z11fseekorigin @"_Z1$7cZ11fseekoriginZ11fseekorigin"(%struct__Z11fseekorigin, %struct__Z11fseekorigin)

declare void @abort()

declare i32 @atexit(void ()*)

declare double @atof(i8*)

declare i32 @atoi(i8*)

declare %struct__Z3_vp* @bsearch(%struct__Z3_vp*, %struct__Z3_vp*, i32, i32, i32 (%struct__Z3_vp*, %struct__Z3_vp*)*)

declare %struct__Z3_vp* @calloc(i32, i32)

declare i32 @exit(i32)

declare i8* @getenv(i8*)

declare void @qsort(%struct__Z3_vp*, i32, i32, i32 (%struct__Z3_vp*, %struct__Z3_vp*)*)

declare i32 @rand()

declare %struct__Z3_vp* @realloc(%struct__Z3_vp*, i32)

declare void @srand(i32)

declare double @strtod(i8*, i8**)

declare i32 @_Z8strtoi32PcPPci(i8*, i8**, i32)

declare i64 @_Z8strtoi64PcPPci(i8*, i8**, i32)

declare i32 @strtol(i8*, i8**, i32)

declare i64 @strtoll(i8*, i8**, i32)

declare i32 @_Z9strtoui32PcPPci(i8*, i8**, i32)

declare i64 @_Z9strtoui64PcPPci(i8*, i8**, i32)

declare i32 @strtoul(i8*, i8**, i32)

declare i64 @strtoull(i8*, i8**, i32)

declare i32 @system(i8*)

declare %struct__Z5DNode* @_ZN3std6assertEPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std18disable$2dassertionsEPZ8MContext"(%struct__Z8MContext*)

declare %struct__Z5DNode* @"_ZN3std17enable$2dassertionsEPZ8MContext"(%struct__Z8MContext*)

declare i32 @isalnum(i32)

declare i32 @isalpha(i32)

declare i32 @iscntrl(i32)

declare i32 @isdigit(i32)

declare i32 @isgraph(i32)

declare i32 @islower(i32)

declare i32 @isprint(i32)

declare i32 @ispunct(i32)

declare i32 @isspace(i32)

declare i32 @isupper(i32)

declare i32 @isxdigit(i32)

declare i32 @tolower(i32)

declare i32 @toupper(i32)

declare i32 @abs(i32)

declare double @acos(double)

declare double @asin(double)

declare double @atan(double)

declare double @atan2(double)

declare double @ceil(double)

declare double @cos(double)

declare float @cosf(float)

declare x86_fp80 @cosl(x86_fp80)

declare %"struct__Z5div$2dt" @div(i32, i32)

declare double @fabs(double)

declare double @floor(double)

declare double @fmod(double, double)

declare double @frexp(double, i32*)

declare double @log(double)

declare double @log10(double)

declare double @modf(double, double*)

declare double @pow(double, double)

declare float @powf(float, float)

declare x86_fp80 @powl(x86_fp80, x86_fp80)

declare double @sin(double)

declare float @sinf(float)

declare x86_fp80 @sinl(x86_fp80)

declare double @sqrt(double)

declare float @sqrtf(float)

declare x86_fp80 @sqrtl(x86_fp80)

declare %struct__Z3_vp* @memchr(%struct__Z3_vp*, i32, i32)

declare i32 @memcmp(%struct__Z3_vp*, %struct__Z3_vp*, i32)

declare %struct__Z3_vp* @memmove(%struct__Z3_vp*, %struct__Z3_vp*, i32)

declare i32 @strcat(i8*, i8*)

declare i32 @strcpy(i8*, i8*)

declare i32 @strcspn(i8*, i8*)

declare i8* @strerror(i32)

declare i32 @strncat(i8*, i8*, i32)

declare i8* @strpbrk(i8*, i8*)

declare i8* @strrchr(i8*, i32)

declare i32 @strspn(i8*, i8*)

declare i8* @strstr(i8*, i8*)

declare i8* @strtok(i8*, i8*)

declare %struct__Z5DNode* @_Z20_CS_GNU_LIBC_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_CS_GNU_LIBPTHREAD_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_CS_LFS64_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_CS_LFS64_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_CS_LFS64_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_CS_LFS64_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_CS_LFS_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_CS_LFS_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_CS_LFS_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_CS_LFS_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z8_CS_PATHPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z31_CS_POSIX_V6_ILP32_OFF32_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z32_CS_POSIX_V6_ILP32_OFF32_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z29_CS_POSIX_V6_ILP32_OFF32_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z34_CS_POSIX_V6_ILP32_OFF32_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z32_CS_POSIX_V6_ILP32_OFFBIG_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z33_CS_POSIX_V6_ILP32_OFFBIG_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_CS_POSIX_V6_ILP32_OFFBIG_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z35_CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_CS_POSIX_V6_LP64_OFF64_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z31_CS_POSIX_V6_LP64_OFF64_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_POSIX_V6_LP64_OFF64_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z33_CS_POSIX_V6_LP64_OFF64_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z32_CS_POSIX_V6_LPBIG_OFFBIG_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z33_CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_CS_POSIX_V6_LPBIG_OFFBIG_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z35_CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z31_CS_POSIX_V7_ILP32_OFF32_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z32_CS_POSIX_V7_ILP32_OFF32_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z29_CS_POSIX_V7_ILP32_OFF32_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z34_CS_POSIX_V7_ILP32_OFF32_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z32_CS_POSIX_V7_ILP32_OFFBIG_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z33_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_CS_POSIX_V7_ILP32_OFFBIG_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z35_CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_CS_POSIX_V7_LP64_OFF64_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z31_CS_POSIX_V7_LP64_OFF64_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_POSIX_V7_LP64_OFF64_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z33_CS_POSIX_V7_LP64_OFF64_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z32_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z33_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_CS_POSIX_V7_LPBIG_OFFBIG_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z35_CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_V5_WIDTH_RESTRICTED_ENVSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z10_CS_V6_ENVPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_V6_WIDTH_RESTRICTED_ENVSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z10_CS_V7_ENVPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_V7_WIDTH_RESTRICTED_ENVSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z27_CS_XBS5_ILP32_OFF32_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_XBS5_ILP32_OFF32_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_CS_XBS5_ILP32_OFF32_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_CS_XBS5_ILP32_OFF32_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_XBS5_ILP32_OFFBIG_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z29_CS_XBS5_ILP32_OFFBIG_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_CS_XBS5_ILP32_OFFBIG_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z31_CS_XBS5_ILP32_OFFBIG_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_CS_XBS5_LP64_OFF64_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z27_CS_XBS5_LP64_OFF64_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z24_CS_XBS5_LP64_OFF64_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z29_CS_XBS5_LP64_OFF64_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z28_CS_XBS5_LPBIG_OFFBIG_CFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z29_CS_XBS5_LPBIG_OFFBIG_LDFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_CS_XBS5_LPBIG_OFFBIG_LIBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z31_CS_XBS5_LPBIG_OFFBIG_LINTFLAGSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_PC_2_SYMLINKSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_PC_ALLOC_SIZE_MINPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_PC_ASYNC_IOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_PC_CHOWN_RESTRICTEDPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_PC_FILESIZEBITSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_PC_LINK_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_PC_MAX_CANONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_PC_MAX_INPUTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_PC_NAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_PC_NO_TRUNCPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_PC_PATH_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_PC_PIPE_BUFPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_PC_PRIO_IOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_PC_REC_INCR_XFER_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_PC_REC_MAX_XFER_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_PC_REC_MIN_XFER_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_PC_REC_XFER_ALIGNPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_PC_SOCK_MAXBUFPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_PC_SYMLINK_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_PC_SYNC_IOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_PC_VDISABLEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_2_CHAR_TERMPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_2_C_BINDPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_2_C_DEVPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_2_C_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_2_FORT_DEVPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_2_FORT_RUNPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_2_LOCALEDEFPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z9_SC_2_PBSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_2_PBS_ACCOUNTINGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_2_PBS_CHECKPOINTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_2_PBS_LOCATEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_2_PBS_MESSAGEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_2_PBS_TRACKPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_2_SW_DEVPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z9_SC_2_UPEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_2_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_ADVISORY_INFOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_AIO_LISTIO_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_AIO_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_AIO_PRIO_DELTA_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_ARG_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_ASYNCHRONOUS_IOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_ATEXIT_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_AVPHYS_PAGESPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_BARRIERSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z8_SC_BASEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_BC_BASE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_BC_DIM_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_BC_SCALE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_BC_STRING_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_CHARCLASS_NAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_CHAR_BITPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_CHAR_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_CHAR_MINPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_CHILD_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_CLK_TCKPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_CLOCK_SELECTIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_COLL_WEIGHTS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_CPUTIMEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_C_LANG_SUPPORTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_C_LANG_SUPPORT_RPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_DELAYTIMER_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_DEVICE_IOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_DEVICE_SPECIFICPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_DEVICE_SPECIFIC_RPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_EQUIV_CLASS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_EXPR_NEST_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_FD_MGMTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z8_SC_FIFOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_FILE_ATTRIBUTESPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_FILE_LOCKINGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_FILE_SYSTEMPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z9_SC_FSYNCPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_GETGR_R_SIZE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_GETPW_R_SIZE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_HOST_NAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_INT_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_INT_MINPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_IOV_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z8_SC_IPV6PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_JOB_CONTROLPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z23_SC_LEVEL1_DCACHE_ASSOCPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_SC_LEVEL1_DCACHE_LINESIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_LEVEL1_DCACHE_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z23_SC_LEVEL1_ICACHE_ASSOCPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_SC_LEVEL1_ICACHE_LINESIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_LEVEL1_ICACHE_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_LEVEL2_CACHE_ASSOCPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_LEVEL2_CACHE_LINESIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_LEVEL2_CACHE_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_LEVEL3_CACHE_ASSOCPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_LEVEL3_CACHE_LINESIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_LEVEL3_CACHE_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_LEVEL4_CACHE_ASSOCPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_LEVEL4_CACHE_LINESIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_LEVEL4_CACHE_SIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_LINE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_LOGIN_NAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_LONG_BITPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_MAPPED_FILESPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_MB_LEN_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_MEMLOCKPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_MEMLOCK_RANGEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_MEMORY_PROTECTIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_MESSAGE_PASSINGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_MONOTONIC_CLOCKPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_MQ_OPEN_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_MQ_PRIO_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_MULTI_PROCESSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_NETWORKINGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_NGROUPS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_NL_ARGMAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_NL_LANGMAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_NL_MSGMAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_NL_NMAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_NL_SETMAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_NL_TEXTMAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_NPROCESSORS_CONFPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_NPROCESSORS_ONLNPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z9_SC_NZEROPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_OPEN_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_PAGESIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_PASS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_PHYS_PAGESPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z7_SC_PIIPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_PII_INTERNETPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_PII_INTERNET_DGRAMPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z23_SC_PII_INTERNET_STREAMPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_PII_OSIPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_PII_OSI_CLTSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_PII_OSI_COTSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_PII_OSI_MPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_PII_SOCKETPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_PII_XTIPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z8_SC_PIPEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z8_SC_POLLPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_PRIORITIZED_IOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z23_SC_PRIORITY_SCHEDULINGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_RAW_SOCKETSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z23_SC_READER_WRITER_LOCKSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_REALTIME_SIGNALSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z10_SC_REGEXPPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_REGEX_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_RE_DUP_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_RTSIG_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_SAVED_IDSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_SCHAR_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_SCHAR_MINPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z10_SC_SELECTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_SEMAPHORESPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_SEM_NSEMS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_SEM_VALUE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_SHARED_MEMORY_OBJECTSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z9_SC_SHELLPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_SHRT_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_SHRT_MINPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_SIGNALSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_SIGQUEUE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_SINGLE_PROCESSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z9_SC_SPAWNPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_SPIN_LOCKSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_SPORADIC_SERVERPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_SSIZE_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_SS_REPL_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_STREAMSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_STREAM_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_SYMLOOP_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_SYNCHRONIZED_IOPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_SYSTEM_DATABASEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_SYSTEM_DATABASE_RPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_THREADSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_THREAD_ATTR_STACKADDRPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_THREAD_ATTR_STACKSIZEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_THREAD_CPUTIMEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z32_SC_THREAD_DESTRUCTOR_ITERATIONSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_THREAD_KEYS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_SC_THREAD_PRIORITY_SCHEDULINGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z23_SC_THREAD_PRIO_INHERITPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z23_SC_THREAD_PRIO_PROTECTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_THREAD_PROCESS_SHAREDPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_SC_THREAD_ROBUST_PRIO_INHERITPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z30_SC_THREAD_ROBUST_PRIO_PROTECTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z25_SC_THREAD_SAFE_FUNCTIONSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_SC_THREAD_SPORADIC_SERVERPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_THREAD_STACK_MINPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_THREAD_THREADS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_TIMEOUTSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z10_SC_TIMERSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_TIMER_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z9_SC_TRACEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z22_SC_TRACE_EVENT_FILTERPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z24_SC_TRACE_EVENT_NAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_TRACE_INHERITPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_TRACE_LOGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_TRACE_NAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_TRACE_SYS_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z24_SC_TRACE_USER_EVENT_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_TTY_NAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z24_SC_TYPED_MEMORY_OBJECTSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_TZNAME_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_T_IOV_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_UCHAR_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_UINT_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_UIO_MAXIOVPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_ULONG_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_USER_GROUPSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_USER_GROUPS_RPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_USHRT_MAXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_V6_ILP32_OFF32PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_V6_ILP32_OFFBIGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_V6_LP64_OFF64PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_V6_LPBIG_OFFBIGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_V7_ILP32_OFF32PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_V7_ILP32_OFFBIGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_V7_LP64_OFF64PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_V7_LPBIG_OFFBIGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z11_SC_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z12_SC_WORD_BITPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z20_SC_XBS5_ILP32_OFF32PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_XBS5_ILP32_OFFBIGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z19_SC_XBS5_LP64_OFF64PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_XBS5_LPBIG_OFFBIGPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z15_SC_XOPEN_CRYPTPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_XOPEN_ENH_I18NPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z16_SC_XOPEN_LEGACYPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z18_SC_XOPEN_REALTIMEPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z26_SC_XOPEN_REALTIME_THREADSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z13_SC_XOPEN_SHMPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_XOPEN_STREAMSPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_XOPEN_UNIXPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z17_SC_XOPEN_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z21_SC_XOPEN_XCU_VERSIONPZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_XOPEN_XPG2PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_XOPEN_XPG3PZ8MContext(%struct__Z8MContext*)

declare %struct__Z5DNode* @_Z14_SC_XOPEN_XPG4PZ8MContext(%struct__Z8MContext*)

declare i32 @__getpgid(i32)

declare void @_exit(i32)

declare i32 @access(i8*, i32)

declare i32 @acct(i8*)

declare i32 @alarm(i32)

declare i32 @brk(%struct__Z3_vp*)

declare i32 @chdir(i8*)

declare i32 @chown(i8*, i32, i32)

declare i32 @chroot(i8*)

declare i32 @close(i32)

declare i32 @confstr(i32, i8*, i32)

declare i8* @ctermid(i8*)

declare i32 @daemon(i32, i32)

declare i32 @dup(i32)

declare i32 @dup2(i32, i32)

declare void @endusershell()

declare i32 @execl(i8*, i8*, ...)

declare i32 @execle(i8*, i8*, ...)

declare i32 @execlp(i8*, i8*, ...)

declare i32 @execv(i8*, i8**)

declare i32 @execve(i8*, i8**, i8**)

declare i32 @execvp(i8*, i8**)

declare i32 @faccessat(i32, i8*, i32, i32)

declare i32 @fchdir(i32)

declare i32 @fchown(i32, i32, i32)

declare i32 @fchownat(i32, i8*, i32, i32, i32)

declare i32 @fdatasync(i32)

declare i32 @fexecve(i32, i8**, i8**)

declare i32 @fork()

declare i32 @fpathconf(i32, i32)

declare i32 @fsync(i32)

declare i32 @ftruncate(i32, i32)

declare i8* @getcwd(i8*, i64)

declare i32 @getdomainname(i8*, i32)

declare i32 @getdtablesize()

declare i32 @getegid()

declare i32 @geteuid()

declare i32 @getgid()

declare i32 @getgroups(i32, i32*)

declare i32 @gethostid()

declare i32 @gethostname(i8*, i32)

declare i8* @getlogin()

declare i32 @getlogin_r(i8*, i32)

declare i32 @getopt(i32, i8**, i8*)

declare i32 @getpagesize()

declare i8* @getpass(i8*)

declare i32 @getpgid(i32)

declare i32 @getpgrp()

declare i32 @getpid()

declare i32 @getppid()

declare i32 @getsid(i32)

declare i32 @getuid()

declare i8* @getusershell()

declare i8* @getwd(i8*)

declare i32 @isatty(i32)

declare i32 @lchown(i8*, i32, i32)

declare i32 @link(i8*, i8*)

declare i32 @linkat(i32, i8*, i32, i8*, i32)

declare i32 @lockf(i32, i32, i32)

declare i32 @lseek(i32, i32, i32)

declare i32 @nice(i32)

declare i32 @pathconf(i8*, i32)

declare i32 @pause()

declare i32 @pipe(i32*)

declare i32 @pread(i32, %struct__Z3_vp*, i32, i32)

declare i32 @profil(i16*, i32, i32, i32)

declare i32 @pwrite(i32, %struct__Z3_vp*, i32, i32)

declare i32 @read(i32, %struct__Z3_vp*, i32)

declare i32 @readlink(i8*, i8*, i32)

declare i32 @readlinkat(i32, i8*, i8*, i32)

declare i32 @revoke(i8*)

declare i32 @rmdir(i8*)

declare %struct__Z3_vp* @sbrk(i32)

declare i32 @setdomainname(i8*, i32)

declare i32 @setegid(i32)

declare i32 @seteuid(i32)

declare i32 @setgid(i32)

declare i32 @sethostid(i32)

declare i32 @sethostname(i8*, i32)

declare i32 @setlogin(i8*)

declare i32 @setpgid(i32, i32)

declare i32 @setpgrp()

declare i32 @setregid(i32, i32)

declare i32 @setreuid(i32, i32)

declare i32 @setsid()

declare i32 @setuid(i32)

declare void @setusershell()

declare i32 @sleep(i32)

declare i32 @symlink(i8*, i8*)

declare i32 @symlinkat(i8*, i32, i8*)

declare void @sync()

declare i32 @syscall(i32, ...)

declare i32 @sysconf(i32)

declare i32 @tcgetpgrp(i32)

declare i32 @tcsetpgrp(i32, i32)

declare i32 @truncate(i8*, i32)

declare i8* @ttyname(i32)

declare i32 @ttyname_r(i32, i8*, i32)

declare i32 @ttyslot()

declare i32 @ualarm(i32, i32)

declare i32 @unlink(i8*)

declare i32 @unlinkat(i32, i8*, i32)

declare i32 @usleep(i32)

declare i32 @vfork()

declare i32 @vhangup()

declare i32 @write(i32, %struct__Z3_vp*, i32)

declare void @"_ZN3std8concepts14add$2drefinementEPZN3std8conceptsE12concept-nodePZN3std8conceptsE12concept-node"(%"struct__ZN3std8concepts12concept$2dnodeE"*, %"struct__ZN3std8concepts12concept$2dnodeE"*)

declare %struct__Z5DNode* @"_ZN3std8concepts13assert$2dreturnEPZ8MContextPZ5DNodePZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std8concepts11def$2dconceptEPZ8MContextPZ5DNodePZ5DNodePZ5DNodez"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @"_ZN3std8concepts17def$2dconcept$2dmacroEPZ8MContextPZ5DNodePZ5DNodePZ5DNodez"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @"_ZN3std8concepts14exists$2dconceptEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @"_ZN3std8concepts17exists$2dconcept$2dfnEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @"_ZN3std8concepts20exists$2dconcept$2dmacroEPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare i32 @"_ZN3std8concepts13get$2dcandidateEPZ8MContextPZ5DNodePPZN3std8conceptsE12concept-nodeiiPcPcPibPc"(%struct__Z8MContext*, %struct__Z5DNode*, %"struct__ZN3std8concepts12concept$2dnodeE"**, i32, i32, i8*, i8*, i32*, i1, i8*)

declare i1 @"_ZN3std8concepts22get$2dconcept$2dmacro$2dnameEPZ8MContextPZ5DNodePZ5DNodebPc"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, i1, i8*)

declare i8** @"_ZN3std8concepts21get$2dtype$2dconcept$2dlistEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %"struct__ZN3std8concepts12concept$2dnodeE"* @"_ZN3std8concepts20get$2dtype$2dconcept$2dmapEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_ZN3std8concepts9implementEPZ8MContextPZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, ...)

declare %struct__Z5DNode* @_ZN3std8concepts10implementsEPZ8MContextPZ5DNodePZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare i1 @"_ZN3std8concepts13implements$2dfnEPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_ZN3std8concepts11instantiateEPZ8MContextPZ5DNodez(%struct__Z8MContext*, %struct__Z5DNode*, ...)

declare %"struct__ZN3std8concepts12concept$2dnodeE"* @"_ZN3std8concepts16make$2dsimple$2dnodeEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare i1 @_ZN3std8concepts7refinesEPZ8MContextPcPc(%struct__Z8MContext*, i8*, i8*)

declare i1 @"_Z25_check$2dconcept$2dAssignablePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z35_check$2dconcept$2dAssociativeContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z36_check$2dconcept$2dBackInsertionSequencePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z36_check$2dconcept$2dBidirectionalIteratorPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z24_check$2dconcept$2dContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z35_check$2dconcept$2dDefaultConstructiblePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z33_check$2dconcept$2dEqualityComparablePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z31_check$2dconcept$2dForwardContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z30_check$2dconcept$2dForwardIteratorPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z37_check$2dconcept$2dFrontInsertionSequencePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z28_check$2dconcept$2dInputIteratorPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z33_check$2dconcept$2dLessThanComparablePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z29_check$2dconcept$2dOutputIteratorPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z39_check$2dconcept$2dPairAssociativeContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z36_check$2dconcept$2dRandomAccessContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z35_check$2dconcept$2dRandomAccessIteratorPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z34_check$2dconcept$2dReversibleContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z23_check$2dconcept$2dSequencePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z41_check$2dconcept$2dSimpleAssociativeContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z41_check$2dconcept$2dSortedAssociativeContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z21_check$2dconcept$2dStructPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z30_check$2dconcept$2dTrivialIteratorPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z19_check$2dconcept$2dTypePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z41_check$2dconcept$2dUniqueAssociativeContainerPZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i1 @"_Z20_check$2dconcept$2dValuePZ5DNodePZ8MContext"(%struct__Z5DNode*, %struct__Z8MContext*)

declare i32 @"_Z19_concept$2dAssignable"()

declare i32 @"_Z29_concept$2dAssociativeContainer"()

declare i32 @"_Z30_concept$2dBackInsertionSequence"()

declare i32 @"_Z30_concept$2dBidirectionalIterator"()

declare i32 @"_Z18_concept$2dContainer"()

declare i32 @"_Z29_concept$2dDefaultConstructible"()

declare i32 @"_Z27_concept$2dEqualityComparable"()

declare i32 @"_Z25_concept$2dForwardContainer"()

declare i32 @"_Z24_concept$2dForwardIterator"()

declare i32 @"_Z31_concept$2dFrontInsertionSequence"()

declare i32 @"_Z22_concept$2dInputIterator"()

declare i32 @"_Z27_concept$2dLessThanComparable"()

declare i32 @"_Z23_concept$2dOutputIterator"()

declare i32 @"_Z33_concept$2dPairAssociativeContainer"()

declare i32 @"_Z30_concept$2dRandomAccessContainer"()

declare i32 @"_Z29_concept$2dRandomAccessIterator"()

declare i32 @"_Z28_concept$2dReversibleContainer"()

declare i32 @"_Z17_concept$2dSequence"()

declare i32 @"_Z35_concept$2dSimpleAssociativeContainer"()

declare i32 @"_Z35_concept$2dSortedAssociativeContainer"()

declare i32 @"_Z15_concept$2dStruct"()

declare i32 @"_Z24_concept$2dTrivialIterator"()

declare i32 @"_Z13_concept$2dType"()

declare i32 @"_Z35_concept$2dUniqueAssociativeContainer"()

declare i32 @"_Z14_concept$2dValue"()

declare %struct__Z5DNode* @"_Z24_impl$2dconcept$2dAssignablePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z34_impl$2dconcept$2dAssociativeContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z35_impl$2dconcept$2dBackInsertionSequencePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z35_impl$2dconcept$2dBidirectionalIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z23_impl$2dconcept$2dContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z34_impl$2dconcept$2dDefaultConstructiblePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z32_impl$2dconcept$2dEqualityComparablePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z30_impl$2dconcept$2dForwardContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z29_impl$2dconcept$2dForwardIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z36_impl$2dconcept$2dFrontInsertionSequencePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z27_impl$2dconcept$2dInputIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z32_impl$2dconcept$2dLessThanComparablePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z28_impl$2dconcept$2dOutputIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z38_impl$2dconcept$2dPairAssociativeContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z35_impl$2dconcept$2dRandomAccessContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z34_impl$2dconcept$2dRandomAccessIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z33_impl$2dconcept$2dReversibleContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z22_impl$2dconcept$2dSequencePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z40_impl$2dconcept$2dSimpleAssociativeContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z40_impl$2dconcept$2dSortedAssociativeContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z20_impl$2dconcept$2dStructPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z29_impl$2dconcept$2dTrivialIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z18_impl$2dconcept$2dTypePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z40_impl$2dconcept$2dUniqueAssociativeContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z19_impl$2dconcept$2dValuePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare i32 @"_Z19_refines$2dAssignableZ9Container"(%struct__Z9Container)

declare i32 @"_Z19_refines$2dAssignableZ15TrivialIterator"(%struct__Z15TrivialIterator)

declare i32 @"_Z19_refines$2dAssignableZ14OutputIterator"(%struct__Z14OutputIterator)

declare i32 @"_Z29_refines$2dAssociativeContainerZ26SimpleAssociativeContainer"(%struct__Z26SimpleAssociativeContainer)

declare i32 @"_Z29_refines$2dAssociativeContainerZ26SortedAssociativeContainer"(%struct__Z26SortedAssociativeContainer)

declare i32 @"_Z29_refines$2dAssociativeContainerZ26UniqueAssociativeContainer"(%struct__Z26UniqueAssociativeContainer)

declare i32 @"_Z29_refines$2dAssociativeContainerZ24PairAssociativeContainer"(%struct__Z24PairAssociativeContainer)

declare i32 @"_Z30_refines$2dBidirectionalIteratorZ20RandomAccessIterator"(%struct__Z20RandomAccessIterator)

declare i32 @"_Z18_refines$2dContainerZ16ForwardContainer"(%struct__Z16ForwardContainer)

declare i32 @"_Z29_refines$2dDefaultConstructibleZ20AssociativeContainer"(%struct__Z20AssociativeContainer)

declare i32 @"_Z29_refines$2dDefaultConstructibleZ15TrivialIterator"(%struct__Z15TrivialIterator)

declare i32 @"_Z29_refines$2dDefaultConstructibleZ14OutputIterator"(%struct__Z14OutputIterator)

declare i32 @"_Z27_refines$2dEqualityComparableZ16ForwardContainer"(%struct__Z16ForwardContainer)

declare i32 @"_Z27_refines$2dEqualityComparableZ15TrivialIterator"(%struct__Z15TrivialIterator)

declare i32 @"_Z25_refines$2dForwardContainerZ19ReversibleContainer"(%struct__Z19ReversibleContainer)

declare i32 @"_Z25_refines$2dForwardContainerZ8Sequence"(%struct__Z8Sequence)

declare i32 @"_Z25_refines$2dForwardContainerZ20AssociativeContainer"(%struct__Z20AssociativeContainer)

declare i32 @"_Z24_refines$2dForwardIteratorZ21BidirectionalIterator"(%struct__Z21BidirectionalIterator)

declare i32 @"_Z22_refines$2dInputIteratorZ15ForwardIterator"(%struct__Z15ForwardIterator)

declare i32 @"_Z27_refines$2dLessThanComparableZ16ForwardContainer"(%struct__Z16ForwardContainer)

declare i32 @"_Z27_refines$2dLessThanComparableZ20RandomAccessIterator"(%struct__Z20RandomAccessIterator)

declare i32 @"_Z28_refines$2dReversibleContainerZ21RandomAccessContainer"(%struct__Z21RandomAccessContainer)

declare i32 @"_Z28_refines$2dReversibleContainerZ26SortedAssociativeContainer"(%struct__Z26SortedAssociativeContainer)

declare i32 @"_Z17_refines$2dSequenceZ22FrontInsertionSequence"(%struct__Z22FrontInsertionSequence)

declare i32 @"_Z17_refines$2dSequenceZ21BackInsertionSequence"(%struct__Z21BackInsertionSequence)

declare i32 @"_Z24_refines$2dTrivialIteratorZ13InputIterator"(%struct__Z13InputIterator)

declare i32 @"_Z13_refines$2dTypeZ6Struct"(%struct__Z6Struct)

declare i32 @"_Z13_refines$2dTypeZ10Assignable"(%struct__Z10Assignable)

declare i32 @"_Z13_refines$2dTypeZ20DefaultConstructible"(%struct__Z20DefaultConstructible)

declare i32 @"_Z13_refines$2dTypeZ18EqualityComparable"(%struct__Z18EqualityComparable)

declare i32 @"_Z13_refines$2dTypeZ18LessThanComparable"(%struct__Z18LessThanComparable)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablec"(i8)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablei"(i32)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignabley"(i8)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablen"(i16)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablep"(i32)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignabler"(i64)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablej"(i32)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablem"(i8)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignableo"(i16)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignableq"(i32)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignables"(i64)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablew"(i32)

declare i32 @"_Z27_impl$2dof$2dconcept$2dAssignablex"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablec"(i8)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablei"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparabley"(i8)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablen"(i16)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablep"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparabler"(i64)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablej"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablem"(i8)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparableo"(i16)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparableq"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparables"(i64)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablew"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparablex"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablec"(i8)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablei"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparabley"(i8)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablen"(i16)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablep"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparabler"(i64)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablej"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablem"(i8)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparableo"(i16)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparableq"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparables"(i64)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablew"(i32)

declare i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparablex"(i32)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypec"(i8)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypei"(i32)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypey"(i8)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypen"(i16)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypep"(i32)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTyper"(i64)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypej"(i32)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypem"(i8)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypeo"(i16)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypeq"(i32)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypes"(i64)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypew"(i32)

declare i32 @"_Z21_impl$2dof$2dconcept$2dTypex"(i32)

declare void @_Z4swapPcPc(i8*, i8*)

declare void @_Z4swapPiPi(i32*, i32*)

declare void @_Z4swapPyPy(i8*, i8*)

declare void @_Z4swapPnPn(i16*, i16*)

declare void @_Z4swapPpPp(i32*, i32*)

declare void @_Z4swapPrPr(i64*, i64*)

declare void @_Z4swapPjPj(i32*, i32*)

declare void @_Z4swapPmPm(i8*, i8*)

declare void @_Z4swapPoPo(i16*, i16*)

declare void @_Z4swapPqPq(i32*, i32*)

declare void @_Z4swapPsPs(i64*, i64*)

declare void @_Z4swapPwPw(i32*, i32*)

declare void @_Z4swapPxPx(i32*, i32*)

declare i1 @_ZN3std7isalnumEc(i8)

declare i1 @_ZN3std7isalphaEc(i8)

declare i1 @_ZN3std7iscntrlEc(i8)

declare i1 @_ZN3std7isdigitEc(i8)

declare i1 @_ZN3std7isgraphEc(i8)

declare i1 @_ZN3std7islowerEc(i8)

declare i1 @_ZN3std7isprintEc(i8)

declare i1 @_ZN3std7ispunctEc(i8)

declare i1 @_ZN3std7isspaceEc(i8)

declare i1 @_ZN3std7isupperEc(i8)

declare i1 @_ZN3std8isxdigitEc(i8)

declare i8 @_ZN3std7tolowerEc(i8)

declare i8 @_ZN3std7toupperEc(i8)

declare i32 @_ZN3std3absEi(i32)

declare i16 @_ZN3std3absEn(i16)

declare i32 @_ZN3std3absEp(i32)

declare i64 @_ZN3std3absEr(i64)

declare float @_ZN3std4acosEf(float)

declare double @_ZN3std4acosEd(double)

declare x86_fp80 @_ZN3std4acosEa(x86_fp80)

declare float @_ZN3std4asinEf(float)

declare double @_ZN3std4asinEd(double)

declare x86_fp80 @_ZN3std4asinEa(x86_fp80)

declare float @_ZN3std4atanEf(float)

declare double @_ZN3std4atanEd(double)

declare x86_fp80 @_ZN3std4atanEa(x86_fp80)

declare float @_ZN3std5atan2Ef(float)

declare double @_ZN3std5atan2Ed(double)

declare x86_fp80 @_ZN3std5atan2Ea(x86_fp80)

declare float @_ZN3std4ceilEf(float)

declare double @_ZN3std4ceilEd(double)

declare x86_fp80 @_ZN3std4ceilEa(x86_fp80)

declare float @_ZN3std3cosEf(float)

declare double @_ZN3std3cosEd(double)

declare x86_fp80 @_ZN3std3cosEa(x86_fp80)

declare float @_ZN3std4coshEf(float)

declare double @_ZN3std4coshEd(double)

declare x86_fp80 @_ZN3std4coshEa(x86_fp80)

declare %"struct__ZN3std7div$2dintE" @_ZN3std3divEii(i32, i32)

declare %"struct__ZN3std9div$2dint16E" @_ZN3std3divEnn(i16, i16)

declare %"struct__ZN3std9div$2dint32E" @_ZN3std3divEpp(i32, i32)

declare %"struct__ZN3std9div$2dint64E" @_ZN3std3divErr(i64, i64)

declare %struct__Z5DNode* @_ZN3std1eEPZ8MContext(%struct__Z8MContext*)

declare float @_ZN3std3expEf(float)

declare double @_ZN3std3expEd(double)

declare x86_fp80 @_ZN3std3expEa(x86_fp80)

declare float @_ZN3std4fabsEf(float)

declare double @_ZN3std4fabsEd(double)

declare x86_fp80 @_ZN3std4fabsEa(x86_fp80)

declare float @_ZN3std5floorEf(float)

declare double @_ZN3std5floorEd(double)

declare x86_fp80 @_ZN3std5floorEa(x86_fp80)

declare float @_ZN3std4fmodEff(float, float)

declare double @_ZN3std4fmodEdd(double, double)

declare x86_fp80 @_ZN3std4fmodEaa(x86_fp80, x86_fp80)

declare float @_ZN3std5frexpEfPi(float, i32*)

declare double @_ZN3std5frexpEdPi(double, i32*)

declare x86_fp80 @_ZN3std5frexpEaPi(x86_fp80, i32*)

declare float @_ZN3std3logEf(float)

declare double @_ZN3std3logEd(double)

declare x86_fp80 @_ZN3std3logEa(x86_fp80)

declare float @_ZN3std5log10Ef(float)

declare double @_ZN3std5log10Ed(double)

declare x86_fp80 @_ZN3std5log10Ea(x86_fp80)

declare %struct__Z5DNode* @"_ZN3std8make$2dabsEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std9make$2dcoshEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std8make$2dexpEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std8make$2dmodEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std9make$2dsinhEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_ZN3std9make$2dtanhEPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare i32 @_ZN3std3modEii(i32, i32)

declare i16 @_ZN3std3modEnn(i16, i16)

declare i32 @_ZN3std3modEpp(i32, i32)

declare i64 @_ZN3std3modErr(i64, i64)

declare i32 @_ZN3std3modEjj(i32, i32)

declare i16 @_ZN3std3modEoo(i16, i16)

declare i32 @_ZN3std3modEqq(i32, i32)

declare i64 @_ZN3std3modEss(i64, i64)

declare float @_ZN3std4modfEfPd(float, double*)

declare double @_ZN3std4modfEdPd(double, double*)

declare x86_fp80 @_ZN3std4modfEaPd(x86_fp80, double*)

declare float @_ZN3std3powEff(float, float)

declare double @_ZN3std3powEdd(double, double)

declare x86_fp80 @_ZN3std3powEaa(x86_fp80, x86_fp80)

declare float @_ZN3std3sinEf(float)

declare double @_ZN3std3sinEd(double)

declare x86_fp80 @_ZN3std3sinEa(x86_fp80)

declare float @_ZN3std4sinhEf(float)

declare double @_ZN3std4sinhEd(double)

declare x86_fp80 @_ZN3std4sinhEa(x86_fp80)

declare float @_ZN3std4sqrtEf(float)

declare double @_ZN3std4sqrtEd(double)

declare x86_fp80 @_ZN3std4sqrtEa(x86_fp80)

declare float @_ZN3std3tanEf(float)

declare double @_ZN3std3tanEd(double)

declare x86_fp80 @_ZN3std3tanEa(x86_fp80)

declare float @_ZN3std4tanhEf(float)

declare double @_ZN3std4tanhEd(double)

declare x86_fp80 @_ZN3std4tanhEa(x86_fp80)

declare %struct__Z5DNode* @_Z8IteratorPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z4PairPZ8MContextPZ5DNodePZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z15ReverseIteratorPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z6TriplePZ8MContextPZ5DNodePZ5DNodePZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z19_Pair$40Z4Type$40Z4TypePZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z28_Triple$40Z4Type$40Z4Type$40Z4TypePZ8MContextPZ5DNodePZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*)

declare i1 @"_Z24make$2dtype$2ddisplay$2dstringPZ8MContextPcPZ5DNodePc"(%struct__Z8MContext*, i8*, %struct__Z5DNode*, i8*)

declare i1 @"_Z24make$2dtype$2ddisplay$2dstringPZ8MContextPcPZ5DNodePZ5DNodePc"(%struct__Z8MContext*, i8*, %struct__Z5DNode*, %struct__Z5DNode*, i8*)

declare i1 @"_Z24make$2dtype$2ddisplay$2dstringPZ8MContextPcPZ5DNodePZ5DNodePZ5DNodePc"(%struct__Z8MContext*, i8*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*, i8*)

declare i1 @"_Z16make$2dtype$2dstringPZ8MContextPcPZ5DNodePc"(%struct__Z8MContext*, i8*, %struct__Z5DNode*, i8*)

declare i1 @"_Z16make$2dtype$2dstringPZ8MContextPcPZ5DNodePZ5DNodePc"(%struct__Z8MContext*, i8*, %struct__Z5DNode*, %struct__Z5DNode*, i8*)

declare i1 @"_Z16make$2dtype$2dstringPZ8MContextPcPZ5DNodePZ5DNodePZ5DNodePc"(%struct__Z8MContext*, i8*, %struct__Z5DNode*, %struct__Z5DNode*, %struct__Z5DNode*, i8*)

declare %struct__Z5DNode* @_Z6VectorPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z29_Vector$40Z18EqualityComparablePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z10_$21$3d$40Z4TypePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z10_$3c$3d$40Z4TypePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z11_$3c$40Z6StructPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z11_$3d$40Z6StructPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z10_$3e$3d$40Z4TypePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z9_$3e$40Z4TypePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z19_relations$40Z6StructPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z12_swap$40Z4TypePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z6ActionPZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @_Z9PredicatePZ8MContextPZ5DNode(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z14_$3c$40Z9ContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z14_$3d$40Z9ContainerPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z49_assign$40Z21BackInsertionSequence$40Z13InputIteratorPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z38_binary$2dsearch$40Z20RandomAccessIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z40_copy$40Z13InputIterator$40Z14OutputIteratorPZ8MContextPZ5DNodePZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z36_equal$2drange$40Z20RandomAccessIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z29_find$2dif$2dnot$40Z13InputIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z25_find$2dif$40Z13InputIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z22_find$40Z13InputIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z26_for$2deach$40Z13InputIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z36_lower$2dbound$40Z20RandomAccessIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z26_max$40Z18LessThanComparablePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z26_min$40Z18LessThanComparablePZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z29_sort$40Z20RandomAccessIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

declare %struct__Z5DNode* @"_Z36_upper$2dbound$40Z20RandomAccessIteratorPZ8MContextPZ5DNode"(%struct__Z8MContext*, %struct__Z5DNode*)

define i32 @_Z3minww(i32 %a, i32 %b) {
entry:
  %0 = alloca i32
  store i32 %a, i32* %0
  %1 = alloca i32
  store i32 %b, i32* %1
  %2 = load i32* %0
  %3 = load i32* %1
  %4 = call i1 @"_Z1$3cww"(i32 %2, i32 %3)
  br i1 %4, label %then, label %else

then:                                             ; preds = %entry
  %5 = load i32* %0
  br label %done_phi

else:                                             ; preds = %entry
  %6 = load i32* %1
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %7 = phi i32 [ %5, %then ], [ %6, %else ]
  ret i32 %7
}

declare %struct__Z7vectori @__retain_struct_0()

define i1 @_Z4initPZ7vectorii(%struct__Z7vectori* %vecp, i32 %cap) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca i32
  store i32 %cap, i32* %1
  %2 = load %struct__Z7vectori** %0
  %3 = getelementptr %struct__Z7vectori* %2, i32 0, i32 0
  %4 = load i32* %1
  %5 = call i32 @"_Z1$2aww"(i32 %4, i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32))
  %6 = call %struct__Z3_vp* @malloc(i32 %5)
  %7 = bitcast %struct__Z3_vp* %6 to i32*
  store i32* %7, i32** %3
  %8 = load %struct__Z7vectori** %0
  %9 = getelementptr %struct__Z7vectori* %8, i32 0, i32 1
  %10 = load %struct__Z7vectori** %0
  %11 = getelementptr %struct__Z7vectori* %10, i32 0, i32 0
  %12 = load i32** %11
  store i32* %12, i32** %9
  %13 = load %struct__Z7vectori** %0
  %14 = getelementptr %struct__Z7vectori* %13, i32 0, i32 2
  store i32 0, i32* %14
  %15 = load %struct__Z7vectori** %0
  %16 = getelementptr %struct__Z7vectori* %15, i32 0, i32 3
  %17 = load i32* %1
  store i32 %17, i32* %16
  ret i1 true
}

define i1 @_Z4initPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = call i1 @_Z4initPZ7vectorii(%struct__Z7vectori* %1, i32 0)
  ret i1 %2
}

define i1 @_Z5emptyPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %3 = load i32* %2
  %4 = call i1 @"_Z1$3dww"(i32 %3, i32 0)
  ret i1 %4
}

define i32 @_Z4sizePZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %3 = load i32* %2
  ret i32 %3
}

define i32 @"_Z8max$2dsizePZ7vectori"(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = call i32 @"_Z1$2dww"(i32 0, i32 1)
  ret i32 %1
}

define %struct__Z5DNode* @"_Z10value$2dtypePZ8MContextPZ7vectori"(%struct__Z8MContext* %mc, %struct__Z5DNode* %vecp) {
entry:
  %0 = alloca %struct__Z8MContext*
  store %struct__Z8MContext* %mc, %struct__Z8MContext** %0
  %1 = alloca %struct__Z5DNode*
  store %struct__Z5DNode* %vecp, %struct__Z5DNode** %1
  %2 = alloca %struct__Z5DNode*
  %3 = load %struct__Z8MContext** %0
  %4 = call %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext* %3, i1 true, %struct__Z5DNode* @_dv135)
  store %struct__Z5DNode* %4, %struct__Z5DNode** %2
  %5 = alloca %struct__Z5DNode*
  %6 = alloca %struct__Z5DNode*
  %7 = alloca %struct__Z5DNode*
  %8 = load %struct__Z5DNode** %2
  ret %struct__Z5DNode* %8
}

define %struct__Z5DNode* @"_Z9size$2dtypePZ8MContextPZ7vectori"(%struct__Z8MContext* %mc, %struct__Z5DNode* %vecp) {
entry:
  %0 = alloca %struct__Z8MContext*
  store %struct__Z8MContext* %mc, %struct__Z8MContext** %0
  %1 = alloca %struct__Z5DNode*
  store %struct__Z5DNode* %vecp, %struct__Z5DNode** %1
  %2 = alloca %struct__Z5DNode*
  %3 = load %struct__Z8MContext** %0
  %4 = call %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext* %3, i1 true, %struct__Z5DNode* @_dv140)
  store %struct__Z5DNode* %4, %struct__Z5DNode** %2
  %5 = alloca %struct__Z5DNode*
  %6 = alloca %struct__Z5DNode*
  %7 = alloca %struct__Z5DNode*
  %8 = load %struct__Z5DNode** %2
  ret %struct__Z5DNode* %8
}

define %struct__Z5DNode* @"_Z15difference$2dtypePZ8MContextPZ7vectori"(%struct__Z8MContext* %mc, %struct__Z5DNode* %vecp) {
entry:
  %0 = alloca %struct__Z8MContext*
  store %struct__Z8MContext* %mc, %struct__Z8MContext** %0
  %1 = alloca %struct__Z5DNode*
  store %struct__Z5DNode* %vecp, %struct__Z5DNode** %1
  %2 = alloca %struct__Z5DNode*
  %3 = load %struct__Z8MContext** %0
  %4 = call %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext* %3, i1 true, %struct__Z5DNode* @_dv144)
  store %struct__Z5DNode* %4, %struct__Z5DNode** %2
  %5 = alloca %struct__Z5DNode*
  %6 = alloca %struct__Z5DNode*
  %7 = alloca %struct__Z5DNode*
  %8 = load %struct__Z5DNode** %2
  ret %struct__Z5DNode* %8
}

define i32 @_Z5frontPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 0
  %3 = load i32** %2
  %4 = load i32* %3
  ret i32 %4
}

define i32 @_Z4backPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %3 = load i32** %2
  %4 = load i32* %3
  ret i32 %4
}

define void @"_Z8pop$2dbackPZ7vectori"(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %3 = alloca i32**
  store i32** %2, i32*** %3
  %4 = load %struct__Z7vectori** %0
  %5 = getelementptr %struct__Z7vectori* %4, i32 0, i32 2
  %6 = load %struct__Z7vectori** %0
  %7 = getelementptr %struct__Z7vectori* %6, i32 0, i32 2
  %8 = load i32* %7
  %9 = call i32 @"_Z1$2dww"(i32 %8, i32 1)
  store i32 %9, i32* %5
  %10 = load %struct__Z7vectori** %0
  %11 = getelementptr %struct__Z7vectori* %10, i32 0, i32 1
  %12 = load %struct__Z7vectori** %0
  %13 = getelementptr %struct__Z7vectori* %12, i32 0, i32 1
  %14 = load i32** %13
  %15 = ptrtoint i32* %14 to i32
  %16 = sub i32 %15, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %17 = inttoptr i32 %16 to i32*
  store i32* %17, i32** %11
  ret void
}

define i1 @_Z6resizePZ7vectoriw(%struct__Z7vectori* %vecp, i32 %new-capacity) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca i32
  store i32 %new-capacity, i32* %1
  %2 = load %struct__Z7vectori** %0
  %3 = getelementptr %struct__Z7vectori* %2, i32 0, i32 3
  %4 = load i32* %3
  %5 = alloca i32
  store i32 %4, i32* %5
  %6 = load %struct__Z7vectori** %0
  %7 = getelementptr %struct__Z7vectori* %6, i32 0, i32 2
  %8 = load i32* %7
  %9 = alloca i32
  store i32 %8, i32* %9
  %10 = load i32* %5
  %11 = load i32* %1
  %12 = call i1 @"_Z1$3dww"(i32 %10, i32 %11)
  br i1 %12, label %then, label %else

then:                                             ; preds = %entry
  ret i1 true

else:                                             ; preds = %entry
  br label %done_then_no_else

done_then_no_else:                                ; preds = %else
  br label %_dale_internal_label_continuelabel_0

_dale_internal_label_continuelabel_0:             ; preds = %then1, %done_then_no_else
  %13 = load i32* %1
  %14 = load i32* %9
  %15 = call i1 @"_Z1$3cww"(i32 %13, i32 %14)
  br i1 %15, label %then1, label %else2

then1:                                            ; preds = %_dale_internal_label_continuelabel_0
  %16 = load %struct__Z7vectori** %0
  call void @"_Z8pop$2dbackPZ7vectori"(%struct__Z7vectori* %16)
  %17 = load i32* %9
  %18 = call i32 @"_Z1$2dww"(i32 %17, i32 1)
  store i32 %18, i32* %9
  br label %_dale_internal_label_continuelabel_0

else2:                                            ; preds = %_dale_internal_label_continuelabel_0
  br label %_dale_internal_label_breaklabel_1

_dale_internal_label_breaklabel_1:                ; preds = %else2
  %19 = load %struct__Z7vectori** %0
  %20 = getelementptr %struct__Z7vectori* %19, i32 0, i32 0
  %21 = load %struct__Z7vectori** %0
  %22 = getelementptr %struct__Z7vectori* %21, i32 0, i32 0
  %23 = load i32** %22
  %24 = bitcast i32* %23 to %struct__Z3_vp*
  %25 = load i32* %1
  %26 = call i32 @"_Z1$2aww"(i32 %25, i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32))
  %27 = call %struct__Z3_vp* @realloc(%struct__Z3_vp* %24, i32 %26)
  %28 = bitcast %struct__Z3_vp* %27 to i32*
  store i32* %28, i32** %20
  %29 = load %struct__Z7vectori** %0
  %30 = getelementptr %struct__Z7vectori* %29, i32 0, i32 3
  %31 = load i32* %1
  store i32 %31, i32* %30
  %32 = load %struct__Z7vectori** %0
  %33 = getelementptr %struct__Z7vectori* %32, i32 0, i32 2
  %34 = load %struct__Z7vectori** %0
  %35 = getelementptr %struct__Z7vectori* %34, i32 0, i32 2
  %36 = load i32* %35
  %37 = load i32* %1
  %38 = call i32 @_Z3minww(i32 %36, i32 %37)
  store i32 %38, i32* %33
  %39 = load %struct__Z7vectori** %0
  %40 = getelementptr %struct__Z7vectori* %39, i32 0, i32 1
  %41 = load %struct__Z7vectori** %0
  %42 = getelementptr %struct__Z7vectori* %41, i32 0, i32 0
  %43 = load i32** %42
  %44 = load %struct__Z7vectori** %0
  %45 = getelementptr %struct__Z7vectori* %44, i32 0, i32 2
  %46 = load i32* %45
  %47 = ptrtoint i32* %43 to i32
  %48 = mul i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32), %46
  %49 = add i32 %47, %48
  %50 = inttoptr i32 %49 to i32*
  store i32* %50, i32** %40
  ret i1 true
}

define i1 @_Z7reservePZ7vectoriw(%struct__Z7vectori* %vecp, i32 %extra) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca i32
  store i32 %extra, i32* %1
  %2 = load %struct__Z7vectori** %0
  %3 = load i32* %1
  %4 = load %struct__Z7vectori** %0
  %5 = getelementptr %struct__Z7vectori* %4, i32 0, i32 3
  %6 = load i32* %5
  %7 = call i32 @"_Z1$2bww"(i32 %3, i32 %6)
  %8 = call i1 @_Z6resizePZ7vectoriw(%struct__Z7vectori* %2, i32 %7)
  ret i1 %8
}

define i1 @"_Z9push$2dbackPZ7vectorii"(%struct__Z7vectori* %vecp, i32 %value) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca i32
  store i32 %value, i32* %1
  %2 = load %struct__Z7vectori** %0
  %3 = getelementptr %struct__Z7vectori* %2, i32 0, i32 3
  %4 = load i32* %3
  %5 = load %struct__Z7vectori** %0
  %6 = getelementptr %struct__Z7vectori* %5, i32 0, i32 2
  %7 = load i32* %6
  %8 = call i1 @"_Z1$3dww"(i32 %4, i32 %7)
  br i1 %8, label %then, label %else

then:                                             ; preds = %entry
  %9 = load %struct__Z7vectori** %0
  %10 = call i1 @_Z7reservePZ7vectoriw(%struct__Z7vectori* %9, i32 1)
  br label %done_phi

else:                                             ; preds = %entry
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %11 = phi i1 [ %10, %then ], [ true, %else ]
  %12 = load %struct__Z7vectori** %0
  %13 = getelementptr %struct__Z7vectori* %12, i32 0, i32 1
  %14 = load i32** %13
  %15 = load i32* %1
  store i32 %15, i32* %14
  %16 = load %struct__Z7vectori** %0
  %17 = getelementptr %struct__Z7vectori* %16, i32 0, i32 1
  %18 = load %struct__Z7vectori** %0
  %19 = getelementptr %struct__Z7vectori* %18, i32 0, i32 1
  %20 = load i32** %19
  %21 = ptrtoint i32* %20 to i32
  %22 = add i32 %21, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %23 = inttoptr i32 %22 to i32*
  store i32* %23, i32** %17
  %24 = load %struct__Z7vectori** %0
  %25 = getelementptr %struct__Z7vectori* %24, i32 0, i32 2
  %26 = load %struct__Z7vectori** %0
  %27 = getelementptr %struct__Z7vectori* %26, i32 0, i32 2
  %28 = load i32* %27
  %29 = call i32 @"_Z1$2bww"(i32 %28, i32 1)
  store i32 %29, i32* %25
  ret i1 true
}

define i32* @"_Z1$24PZ7vectorii"(%struct__Z7vectori* %vecp, i32 %index) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca i32
  store i32 %index, i32* %1
  %2 = load %struct__Z7vectori** %0
  %3 = getelementptr %struct__Z7vectori* %2, i32 0, i32 0
  %4 = load i32** %3
  %5 = load i32* %1
  %6 = ptrtoint i32* %4 to i32
  %7 = mul i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32), %5
  %8 = add i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  ret i32* %9
}

define i32* @"_Z1$24PZ7vectoriw"(%struct__Z7vectori* %vecp, i32 %index) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca i32
  store i32 %index, i32* %1
  %2 = load %struct__Z7vectori** %0
  %3 = getelementptr %struct__Z7vectori* %2, i32 0, i32 0
  %4 = load i32** %3
  %5 = load i32* %1
  %6 = ptrtoint i32* %4 to i32
  %7 = mul i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32), %5
  %8 = add i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  ret i32* %9
}

declare %struct__Z17iteratorZ7vectori @__retain_struct_1()

define %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %3 = load %struct__Z7vectori** %0
  store %struct__Z7vectori* %3, %struct__Z7vectori** %2
  %4 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %5 = load %struct__Z7vectori** %0
  %6 = getelementptr %struct__Z7vectori* %5, i32 0, i32 0
  %7 = load i32** %6
  store i32* %7, i32** %4
  %8 = load %struct__Z17iteratorZ7vectori* %1
  ret %struct__Z17iteratorZ7vectori %8
}

define %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %3 = load %struct__Z7vectori** %0
  store %struct__Z7vectori* %3, %struct__Z7vectori** %2
  %4 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %5 = load %struct__Z7vectori** %0
  %6 = getelementptr %struct__Z7vectori* %5, i32 0, i32 1
  %7 = load i32** %6
  store i32* %7, i32** %4
  %8 = load %struct__Z17iteratorZ7vectori* %1
  ret %struct__Z17iteratorZ7vectori %8
}

define %struct__Z5DNode* @"_Z10value$2dtypePZ8MContextPZ17iteratorZ7vectori"(%struct__Z8MContext* %mc, %struct__Z5DNode* %veciter) {
entry:
  %0 = alloca %struct__Z8MContext*
  store %struct__Z8MContext* %mc, %struct__Z8MContext** %0
  %1 = alloca %struct__Z5DNode*
  store %struct__Z5DNode* %veciter, %struct__Z5DNode** %1
  %2 = alloca %struct__Z5DNode*
  %3 = load %struct__Z8MContext** %0
  %4 = call %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext* %3, i1 true, %struct__Z5DNode* @_dv148)
  store %struct__Z5DNode* %4, %struct__Z5DNode** %2
  %5 = alloca %struct__Z5DNode*
  %6 = alloca %struct__Z5DNode*
  %7 = alloca %struct__Z5DNode*
  %8 = load %struct__Z5DNode** %2
  ret %struct__Z5DNode* %8
}

define %struct__Z5DNode* @"_Z13distance$2dtypePZ8MContextPZ17iteratorZ7vectori"(%struct__Z8MContext* %mc, %struct__Z5DNode* %veciter) {
entry:
  %0 = alloca %struct__Z8MContext*
  store %struct__Z8MContext* %mc, %struct__Z8MContext** %0
  %1 = alloca %struct__Z5DNode*
  store %struct__Z5DNode* %veciter, %struct__Z5DNode** %1
  %2 = alloca %struct__Z5DNode*
  %3 = load %struct__Z8MContext** %0
  %4 = call %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext* %3, i1 true, %struct__Z5DNode* @_dv151)
  store %struct__Z5DNode* %4, %struct__Z5DNode** %2
  %5 = alloca %struct__Z5DNode*
  %6 = alloca %struct__Z5DNode*
  %7 = alloca %struct__Z5DNode*
  %8 = load %struct__Z5DNode** %2
  ret %struct__Z5DNode* %8
}

define i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter, %struct__Z17iteratorZ7vectori* %0
  %1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %2 = load i32** %1
  %3 = load i32* %2
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 %3
}

define i32* @_Z6sourceZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter, %struct__Z17iteratorZ7vectori* %0
  %1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %2 = load i32** %1
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32* %2
}

define i1 @_Z4sinkZ17iteratorZ7vectorii(%struct__Z17iteratorZ7vectori %iter, i32 %v) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca i32
  store i32 %v, i32* %1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = load i32** %2
  %4 = load i32* %1
  store i32 %4, i32* %3
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i1 true
}

define %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter, %struct__Z17iteratorZ7vectori* %0
  %1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = load i32** %2
  %4 = ptrtoint i32* %3 to i32
  %5 = add i32 %4, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %6 = inttoptr i32 %5 to i32*
  store i32* %6, i32** %1
  %7 = load %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %7
}

define %struct__Z17iteratorZ7vectori @_Z11predecessorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter, %struct__Z17iteratorZ7vectori* %0
  %1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = load i32** %2
  %4 = ptrtoint i32* %3 to i32
  %5 = sub i32 %4, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %6 = inttoptr i32 %5 to i32*
  store i32* %6, i32** %1
  %7 = load %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %7
}

define i1 @"_Z1$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori %iter2) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter2, %struct__Z17iteratorZ7vectori* %1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = load i32** %2
  %4 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %5 = load i32** %4
  %6 = icmp eq i32* %3, %5
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i1 %6
}

define i32 @"_Z21_impl$2dof$2dconcept$2dTypeZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i1 @"_Z2$21$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %b, %struct__Z17iteratorZ7vectori* %1
  %2 = load %struct__Z17iteratorZ7vectori* %0
  %3 = load %struct__Z17iteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %2, %struct__Z17iteratorZ7vectori %3)
  %5 = call i1 @_Z3notb(i1 %4)
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i1 %5
}

define i1 @"_Z1$3cZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori %iter2) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter2, %struct__Z17iteratorZ7vectori* %1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = load i32** %2
  %4 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %5 = load i32** %4
  %6 = icmp ult i32* %3, %5
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i1 %6
}

define i1 @"_Z2$3c$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %b, %struct__Z17iteratorZ7vectori* %1
  %2 = load %struct__Z17iteratorZ7vectori* %0
  %3 = load %struct__Z17iteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3cZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %2, %struct__Z17iteratorZ7vectori %3)
  br i1 %4, label %then, label %else

then:                                             ; preds = %entry
  br label %done_phi

else:                                             ; preds = %entry
  %5 = load %struct__Z17iteratorZ7vectori* %1
  %6 = load %struct__Z17iteratorZ7vectori* %0
  %7 = call i1 @"_Z1$3cZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %5, %struct__Z17iteratorZ7vectori %6)
  %8 = call i1 @_Z3notb(i1 %7)
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ true, %then ], [ %8, %else ]
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i1 %9
}

define i1 @"_Z1$3eZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %b, %struct__Z17iteratorZ7vectori* %1
  %2 = load %struct__Z17iteratorZ7vectori* %0
  %3 = load %struct__Z17iteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3cZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %2, %struct__Z17iteratorZ7vectori %3)
  %5 = call i1 @_Z3notb(i1 %4)
  br i1 %5, label %then, label %else

then:                                             ; preds = %entry
  %6 = load %struct__Z17iteratorZ7vectori* %1
  %7 = load %struct__Z17iteratorZ7vectori* %0
  %8 = call i1 @"_Z1$3cZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %6, %struct__Z17iteratorZ7vectori %7)
  br label %done_phi

else:                                             ; preds = %entry
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ %8, %then ], [ false, %else ]
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i1 %9
}

define i1 @"_Z2$3e$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %b, %struct__Z17iteratorZ7vectori* %1
  %2 = load %struct__Z17iteratorZ7vectori* %0
  %3 = load %struct__Z17iteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3cZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %2, %struct__Z17iteratorZ7vectori %3)
  %5 = call i1 @_Z3notb(i1 %4)
  br i1 %5, label %then, label %else

then:                                             ; preds = %entry
  br label %done_phi

else:                                             ; preds = %entry
  %6 = load %struct__Z17iteratorZ7vectori* %1
  %7 = load %struct__Z17iteratorZ7vectori* %0
  %8 = call i1 @"_Z1$3cZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %6, %struct__Z17iteratorZ7vectori %7)
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ true, %then ], [ %8, %else ]
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i1 %9
}

define %struct__Z17iteratorZ7vectori @"_Z1$2bZ17iteratorZ7vectoriw"(%struct__Z17iteratorZ7vectori %iter1, i32 %n) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca i32
  store i32 %n, i32* %1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %4 = load i32** %3
  %5 = load i32* %1
  %6 = ptrtoint i32* %4 to i32
  %7 = mul i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32), %5
  %8 = add i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  store i32* %9, i32** %2
  %10 = load %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %10
}

define %struct__Z17iteratorZ7vectori @"_Z1$2dZ17iteratorZ7vectoriw"(%struct__Z17iteratorZ7vectori %iter1, i32 %n) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca i32
  store i32 %n, i32* %1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %4 = load i32** %3
  %5 = load i32* %1
  %6 = ptrtoint i32* %4 to i32
  %7 = mul i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32), %5
  %8 = sub i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  store i32* %9, i32** %2
  %10 = load %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %10
}

define i32 @_Z8distanceZ17iteratorZ7vectoriZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori %iter2) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter1, %struct__Z17iteratorZ7vectori* %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter2, %struct__Z17iteratorZ7vectori* %1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  %3 = load i32** %2
  %4 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %5 = load i32** %4
  %6 = ptrtoint i32* %3 to i32
  %7 = ptrtoint i32* %5 to i32
  %8 = sub i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  %10 = ptrtoint i32* %9 to i32
  %11 = call i32 @abs(i32 %10)
  %12 = call i32 @"_Z1$2fii"(i32 %11, i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32))
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 %12
}

declare %struct__Z18riteratorZ7vectori @__retain_struct_2()

define %struct__Z18riteratorZ7vectori @_Z6rbeginPZ7vectori(%struct__Z7vectori* %vec) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vec, %struct__Z7vectori** %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  %2 = alloca %struct__Z18riteratorZ7vectori
  %3 = getelementptr %struct__Z18riteratorZ7vectori* %2, i32 0, i32 0
  %4 = load %struct__Z7vectori** %0
  %5 = getelementptr %struct__Z7vectori* %4, i32 0, i32 1
  %6 = load i32** %5
  store i32* %6, i32** %3
  %7 = load %struct__Z18riteratorZ7vectori* %2
  store %struct__Z18riteratorZ7vectori %7, %struct__Z18riteratorZ7vectori* %1
  %8 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %7, %struct__Z18riteratorZ7vectori* %8
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %8, i32 0, i32 0
  %9 = load %struct__Z7vectori** %0
  %10 = getelementptr %struct__Z7vectori* %9, i32 0, i32 2
  %11 = load i32* %10
  %12 = call i1 @"_Z2$21$3dww"(i32 %11, i32 0)
  br i1 %12, label %then, label %else

then:                                             ; preds = %entry
  %13 = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %14 = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %15 = load i32** %14
  %16 = ptrtoint i32* %15 to i32
  %17 = sub i32 %16, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %18 = inttoptr i32 %17 to i32*
  store i32* %18, i32** %13
  br label %done_phi

else:                                             ; preds = %entry
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %19 = phi i32 [ 0, %then ], [ 0, %else ]
  %20 = load %struct__Z18riteratorZ7vectori* %1
  ret %struct__Z18riteratorZ7vectori %20
}

define %struct__Z18riteratorZ7vectori @_Z4rendPZ7vectori(%struct__Z7vectori* %vec) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vec, %struct__Z7vectori** %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  %2 = alloca %struct__Z18riteratorZ7vectori
  %3 = getelementptr %struct__Z18riteratorZ7vectori* %2, i32 0, i32 0
  %4 = load %struct__Z7vectori** %0
  %5 = getelementptr %struct__Z7vectori* %4, i32 0, i32 0
  %6 = load i32** %5
  store i32* %6, i32** %3
  %7 = load %struct__Z18riteratorZ7vectori* %2
  store %struct__Z18riteratorZ7vectori %7, %struct__Z18riteratorZ7vectori* %1
  %8 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %7, %struct__Z18riteratorZ7vectori* %8
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %8, i32 0, i32 0
  %9 = load %struct__Z18riteratorZ7vectori* %1
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  ret %struct__Z18riteratorZ7vectori %9
}

define %struct__Z5DNode* @"_Z10value$2dtypePZ8MContextPZ18riteratorZ7vectori"(%struct__Z8MContext* %mc, %struct__Z5DNode* %veciter) {
entry:
  %0 = alloca %struct__Z8MContext*
  store %struct__Z8MContext* %mc, %struct__Z8MContext** %0
  %1 = alloca %struct__Z5DNode*
  store %struct__Z5DNode* %veciter, %struct__Z5DNode** %1
  %2 = alloca %struct__Z5DNode*
  %3 = load %struct__Z8MContext** %0
  %4 = call %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext* %3, i1 true, %struct__Z5DNode* @_dv154)
  store %struct__Z5DNode* %4, %struct__Z5DNode** %2
  %5 = alloca %struct__Z5DNode*
  %6 = alloca %struct__Z5DNode*
  %7 = alloca %struct__Z5DNode*
  %8 = load %struct__Z5DNode** %2
  ret %struct__Z5DNode* %8
}

define %struct__Z5DNode* @"_Z13distance$2dtypePZ8MContextPZ18riteratorZ7vectori"(%struct__Z8MContext* %mc, %struct__Z5DNode* %veciter) {
entry:
  %0 = alloca %struct__Z8MContext*
  store %struct__Z8MContext* %mc, %struct__Z8MContext** %0
  %1 = alloca %struct__Z5DNode*
  store %struct__Z5DNode* %veciter, %struct__Z5DNode** %1
  %2 = alloca %struct__Z5DNode*
  %3 = load %struct__Z8MContext** %0
  %4 = call %struct__Z5DNode* @_ZN3std6macros4copyEPZ8MContextbPZ5DNode(%struct__Z8MContext* %3, i1 true, %struct__Z5DNode* @_dv157)
  store %struct__Z5DNode* %4, %struct__Z5DNode** %2
  %5 = alloca %struct__Z5DNode*
  %6 = alloca %struct__Z5DNode*
  %7 = alloca %struct__Z5DNode*
  %8 = load %struct__Z5DNode** %2
  ret %struct__Z5DNode* %8
}

define i32 @"_Z7$40sourceZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter, %struct__Z18riteratorZ7vectori* %0
  %1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %2 = load i32** %1
  %3 = load i32* %2
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 %3
}

define i32* @_Z6sourceZ18riteratorZ7vectori(%struct__Z18riteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter, %struct__Z18riteratorZ7vectori* %0
  %1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %2 = load i32** %1
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32* %2
}

define i1 @_Z4sinkZ18riteratorZ7vectorii(%struct__Z18riteratorZ7vectori %iter, i32 %sinker) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca i32
  store i32 %sinker, i32* %1
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = load i32** %2
  %4 = load i32* %1
  store i32 %4, i32* %3
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i1 true
}

define %struct__Z18riteratorZ7vectori @_Z9successorZ18riteratorZ7vectori(%struct__Z18riteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter, %struct__Z18riteratorZ7vectori* %0
  %1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = load i32** %2
  %4 = ptrtoint i32* %3 to i32
  %5 = sub i32 %4, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %6 = inttoptr i32 %5 to i32*
  store i32* %6, i32** %1
  %7 = load %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret %struct__Z18riteratorZ7vectori %7
}

define %struct__Z18riteratorZ7vectori @_Z11predecessorZ18riteratorZ7vectori(%struct__Z18riteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter, %struct__Z18riteratorZ7vectori* %0
  %1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = load i32** %2
  %4 = ptrtoint i32* %3 to i32
  %5 = add i32 %4, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %6 = inttoptr i32 %5 to i32*
  store i32* %6, i32** %1
  %7 = load %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret %struct__Z18riteratorZ7vectori %7
}

define i1 @"_Z1$3dZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori %iter2) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter2, %struct__Z18riteratorZ7vectori* %1
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = load i32** %2
  %4 = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %5 = load i32** %4
  %6 = icmp eq i32* %3, %5
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i1 %6
}

define i32 @"_Z21_impl$2dof$2dconcept$2dTypeZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i1 @"_Z2$21$3dZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %b, %struct__Z18riteratorZ7vectori* %1
  %2 = load %struct__Z18riteratorZ7vectori* %0
  %3 = load %struct__Z18riteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3dZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %2, %struct__Z18riteratorZ7vectori %3)
  %5 = call i1 @_Z3notb(i1 %4)
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i1 %5
}

define i1 @"_Z1$3cZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori %iter2) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter2, %struct__Z18riteratorZ7vectori* %1
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = load i32** %2
  %4 = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %5 = load i32** %4
  %6 = ptrtoint i32* %5 to i32
  %7 = sub i32 %6, ptrtoint (i32* getelementptr (i32* null, i32 1) to i32)
  %8 = inttoptr i32 %7 to i32*
  %9 = icmp ugt i32* %3, %8
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i1 %9
}

define i1 @"_Z2$3c$3dZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %b, %struct__Z18riteratorZ7vectori* %1
  %2 = load %struct__Z18riteratorZ7vectori* %0
  %3 = load %struct__Z18riteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3cZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %2, %struct__Z18riteratorZ7vectori %3)
  br i1 %4, label %then, label %else

then:                                             ; preds = %entry
  br label %done_phi

else:                                             ; preds = %entry
  %5 = load %struct__Z18riteratorZ7vectori* %1
  %6 = load %struct__Z18riteratorZ7vectori* %0
  %7 = call i1 @"_Z1$3cZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %5, %struct__Z18riteratorZ7vectori %6)
  %8 = call i1 @_Z3notb(i1 %7)
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ true, %then ], [ %8, %else ]
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i1 %9
}

define i1 @"_Z1$3eZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %b, %struct__Z18riteratorZ7vectori* %1
  %2 = load %struct__Z18riteratorZ7vectori* %0
  %3 = load %struct__Z18riteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3cZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %2, %struct__Z18riteratorZ7vectori %3)
  %5 = call i1 @_Z3notb(i1 %4)
  br i1 %5, label %then, label %else

then:                                             ; preds = %entry
  %6 = load %struct__Z18riteratorZ7vectori* %1
  %7 = load %struct__Z18riteratorZ7vectori* %0
  %8 = call i1 @"_Z1$3cZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %6, %struct__Z18riteratorZ7vectori %7)
  br label %done_phi

else:                                             ; preds = %entry
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ %8, %then ], [ false, %else ]
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i1 %9
}

define i1 @"_Z2$3e$3dZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori %b) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %b, %struct__Z18riteratorZ7vectori* %1
  %2 = load %struct__Z18riteratorZ7vectori* %0
  %3 = load %struct__Z18riteratorZ7vectori* %1
  %4 = call i1 @"_Z1$3cZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %2, %struct__Z18riteratorZ7vectori %3)
  %5 = call i1 @_Z3notb(i1 %4)
  br i1 %5, label %then, label %else

then:                                             ; preds = %entry
  br label %done_phi

else:                                             ; preds = %entry
  %6 = load %struct__Z18riteratorZ7vectori* %1
  %7 = load %struct__Z18riteratorZ7vectori* %0
  %8 = call i1 @"_Z1$3cZ18riteratorZ7vectoriZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %6, %struct__Z18riteratorZ7vectori %7)
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ true, %then ], [ %8, %else ]
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i1 %9
}

define %struct__Z18riteratorZ7vectori @"_Z1$2bZ18riteratorZ7vectoriw"(%struct__Z18riteratorZ7vectori %iter1, i32 %n) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca i32
  store i32 %n, i32* %1
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %4 = load i32** %3
  %5 = load i32* %1
  %6 = ptrtoint i32* %4 to i32
  %7 = mul i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32), %5
  %8 = add i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  store i32* %9, i32** %2
  %10 = load %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret %struct__Z18riteratorZ7vectori %10
}

define %struct__Z18riteratorZ7vectori @"_Z1$2dZ18riteratorZ7vectoriw"(%struct__Z18riteratorZ7vectori %iter1, i32 %n) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca i32
  store i32 %n, i32* %1
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %4 = load i32** %3
  %5 = load i32* %1
  %6 = ptrtoint i32* %4 to i32
  %7 = mul i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32), %5
  %8 = sub i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  store i32* %9, i32** %2
  %10 = load %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret %struct__Z18riteratorZ7vectori %10
}

define i32 @_Z8distanceZ18riteratorZ7vectoriZ18riteratorZ7vectori(%struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori %iter2) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter1, %struct__Z18riteratorZ7vectori* %0
  %1 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %iter2, %struct__Z18riteratorZ7vectori* %1
  %2 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  %3 = load i32** %2
  %4 = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %5 = load i32** %4
  %6 = ptrtoint i32* %3 to i32
  %7 = ptrtoint i32* %5 to i32
  %8 = sub i32 %6, %7
  %9 = inttoptr i32 %8 to i32*
  %10 = ptrtoint i32* %9 to i32
  %11 = call i32 @abs(i32 %10)
  %12 = call i32 @"_Z1$2fii"(i32 %11, i32 ptrtoint (i32* getelementptr (i32* null, i32 1) to i32))
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 %12
}

define %struct__Z17iteratorZ7vectori @_Z6insertPZ7vectoriZ17iteratorZ7vectorii(%struct__Z7vectori* %unused, %struct__Z17iteratorZ7vectori %iter, i32 %value) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %unused, %struct__Z7vectori** %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter, %struct__Z17iteratorZ7vectori* %1
  %2 = alloca i32
  store i32 %value, i32* %2
  %3 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %4 = load %struct__Z7vectori** %3
  %5 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %4, %struct__Z7vectori** %5
  %6 = load %struct__Z17iteratorZ7vectori* %1
  %7 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %6, %struct__Z17iteratorZ7vectori* %7
  %8 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %6, %struct__Z17iteratorZ7vectori* %8
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %8, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %8, i32 0, i32 1
  %9 = load %struct__Z17iteratorZ7vectori* %1
  %10 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %9)
  %11 = alloca i32
  store i32 %10, i32* %11
  %12 = load %struct__Z17iteratorZ7vectori* %1
  %13 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %12)
  %14 = alloca i32
  store i32 %13, i32* %14
  %15 = load %struct__Z7vectori** %5
  %16 = getelementptr %struct__Z7vectori* %15, i32 0, i32 3
  %17 = load i32* %16
  %18 = load %struct__Z7vectori** %5
  %19 = call i32 @_Z4sizePZ7vectori(%struct__Z7vectori* %18)
  %20 = call i1 @"_Z2$3c$3dww"(i32 %17, i32 %19)
  br i1 %20, label %then, label %else

then:                                             ; preds = %entry
  %21 = load %struct__Z7vectori** %5
  %22 = load %struct__Z7vectori** %5
  %23 = getelementptr %struct__Z7vectori* %22, i32 0, i32 3
  %24 = load i32* %23
  %25 = call i1 @_Z7reservePZ7vectoriw(%struct__Z7vectori* %21, i32 %24)
  br label %done_phi

else:                                             ; preds = %entry
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %26 = phi i1 [ %25, %then ], [ true, %else ]
  %27 = load %struct__Z7vectori** %5
  %28 = call i32 @_Z4sizePZ7vectori(%struct__Z7vectori* %27)
  %29 = call i1 @"_Z1$3dww"(i32 0, i32 %28)
  br i1 %29, label %then2, label %else3

then2:                                            ; preds = %done_phi
  %30 = load %struct__Z7vectori** %5
  %31 = load i32* %2
  %32 = call i1 @"_Z9push$2dbackPZ7vectorii"(%struct__Z7vectori* %30, i32 %31)
  %33 = load %struct__Z17iteratorZ7vectori* %1
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %7, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %7, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf7 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %33

else3:                                            ; preds = %done_phi
  br label %done_then_no_else

done_then_no_else:                                ; preds = %else3
  %34 = load %struct__Z7vectori** %5
  %35 = getelementptr %struct__Z7vectori* %34, i32 0, i32 2
  %36 = load %struct__Z7vectori** %5
  %37 = getelementptr %struct__Z7vectori* %36, i32 0, i32 2
  %38 = load i32* %37
  %39 = call i32 @"_Z1$2bww"(i32 %38, i32 1)
  store i32 %39, i32* %35
  br label %_dale_internal_label_continuelabel_2

_dale_internal_label_continuelabel_2:             ; preds = %then8, %done_then_no_else
  %40 = load %struct__Z17iteratorZ7vectori* %1
  %41 = load %struct__Z7vectori** %5
  %42 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %41)
  %43 = call i1 @"_Z2$21$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %40, %struct__Z17iteratorZ7vectori %42)
  br i1 %43, label %then8, label %else9

then8:                                            ; preds = %_dale_internal_label_continuelabel_2
  %44 = load %struct__Z17iteratorZ7vectori* %1
  %45 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %44)
  store %struct__Z17iteratorZ7vectori %45, %struct__Z17iteratorZ7vectori* %1
  %46 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %45, %struct__Z17iteratorZ7vectori* %46
  %asdf10 = getelementptr %struct__Z17iteratorZ7vectori* %46, i32 0, i32 0
  %asdf11 = getelementptr %struct__Z17iteratorZ7vectori* %46, i32 0, i32 1
  %47 = load %struct__Z17iteratorZ7vectori* %1
  %48 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %47)
  store i32 %48, i32* %11
  %49 = load %struct__Z17iteratorZ7vectori* %1
  %50 = load i32* %14
  %51 = call i1 @_Z4sinkZ17iteratorZ7vectorii(%struct__Z17iteratorZ7vectori %49, i32 %50)
  %52 = load i32* %11
  store i32 %52, i32* %14
  br label %_dale_internal_label_continuelabel_2

else9:                                            ; preds = %_dale_internal_label_continuelabel_2
  br label %_dale_internal_label_breaklabel_3

_dale_internal_label_breaklabel_3:                ; preds = %else9
  %53 = load %struct__Z17iteratorZ7vectori* %1
  %54 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %53)
  store %struct__Z17iteratorZ7vectori %54, %struct__Z17iteratorZ7vectori* %1
  %55 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %54, %struct__Z17iteratorZ7vectori* %55
  %asdf12 = getelementptr %struct__Z17iteratorZ7vectori* %55, i32 0, i32 0
  %asdf13 = getelementptr %struct__Z17iteratorZ7vectori* %55, i32 0, i32 1
  %56 = load %struct__Z7vectori** %5
  %57 = getelementptr %struct__Z7vectori* %56, i32 0, i32 1
  %58 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %59 = load i32** %58
  store i32* %59, i32** %57
  %60 = load %struct__Z17iteratorZ7vectori* %7
  %61 = load i32* %2
  %62 = call i1 @_Z4sinkZ17iteratorZ7vectorii(%struct__Z17iteratorZ7vectori %60, i32 %61)
  %63 = load %struct__Z17iteratorZ7vectori* %7
  %64 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %63)
  %asdf14 = getelementptr %struct__Z17iteratorZ7vectori* %7, i32 0, i32 0
  %asdf15 = getelementptr %struct__Z17iteratorZ7vectori* %7, i32 0, i32 1
  %asdf16 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf17 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %64
}

define %struct__Z17iteratorZ7vectori @_Z5erasePZ7vectoriZ17iteratorZ7vectori(%struct__Z7vectori* %unused, %struct__Z17iteratorZ7vectori %iter) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %unused, %struct__Z7vectori** %0
  %1 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %iter, %struct__Z17iteratorZ7vectori* %1
  %2 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  %3 = load i32** %2
  %4 = alloca i32*
  store i32* %3, i32** %4
  %5 = alloca %struct__Z17iteratorZ7vectori
  %6 = alloca %struct__Z17iteratorZ7vectori
  %7 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %8 = load %struct__Z7vectori** %7
  %9 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %8, %struct__Z7vectori** %9
  %10 = load %struct__Z17iteratorZ7vectori* %1
  %11 = load %struct__Z7vectori** %9
  %12 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %11)
  %13 = call i1 @"_Z1$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %10, %struct__Z17iteratorZ7vectori %12)
  br i1 %13, label %then, label %else

then:                                             ; preds = %entry
  %14 = load %struct__Z17iteratorZ7vectori* %1
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %6, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %6, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %5, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %5, i32 0, i32 1
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %14

else:                                             ; preds = %entry
  br label %done_then_no_else

done_then_no_else:                                ; preds = %else
  %15 = load %struct__Z7vectori** %9
  %16 = getelementptr %struct__Z7vectori* %15, i32 0, i32 2
  %17 = load %struct__Z7vectori** %9
  %18 = getelementptr %struct__Z7vectori* %17, i32 0, i32 2
  %19 = load i32* %18
  %20 = call i32 @"_Z1$2dww"(i32 %19, i32 1)
  store i32 %20, i32* %16
  %21 = load %struct__Z17iteratorZ7vectori* %1
  %22 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %21)
  store %struct__Z17iteratorZ7vectori %22, %struct__Z17iteratorZ7vectori* %5
  %23 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %22, %struct__Z17iteratorZ7vectori* %23
  %asdf6 = getelementptr %struct__Z17iteratorZ7vectori* %23, i32 0, i32 0
  %asdf7 = getelementptr %struct__Z17iteratorZ7vectori* %23, i32 0, i32 1
  %24 = load %struct__Z17iteratorZ7vectori* %1
  store %struct__Z17iteratorZ7vectori %24, %struct__Z17iteratorZ7vectori* %6
  %25 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %24, %struct__Z17iteratorZ7vectori* %25
  %asdf8 = getelementptr %struct__Z17iteratorZ7vectori* %25, i32 0, i32 0
  %asdf9 = getelementptr %struct__Z17iteratorZ7vectori* %25, i32 0, i32 1
  br label %_dale_internal_label_continuelabel_4

_dale_internal_label_continuelabel_4:             ; preds = %then10, %done_then_no_else
  %26 = load %struct__Z17iteratorZ7vectori* %5
  %27 = load %struct__Z7vectori** %9
  %28 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %27)
  %29 = call i1 @"_Z2$21$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %26, %struct__Z17iteratorZ7vectori %28)
  br i1 %29, label %then10, label %else11

then10:                                           ; preds = %_dale_internal_label_continuelabel_4
  %30 = load %struct__Z17iteratorZ7vectori* %5
  store %struct__Z17iteratorZ7vectori %30, %struct__Z17iteratorZ7vectori* %6
  %31 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %30, %struct__Z17iteratorZ7vectori* %31
  %asdf12 = getelementptr %struct__Z17iteratorZ7vectori* %31, i32 0, i32 0
  %asdf13 = getelementptr %struct__Z17iteratorZ7vectori* %31, i32 0, i32 1
  %32 = load %struct__Z17iteratorZ7vectori* %1
  %33 = load %struct__Z17iteratorZ7vectori* %5
  %34 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %33)
  %35 = call i1 @_Z4sinkZ17iteratorZ7vectorii(%struct__Z17iteratorZ7vectori %32, i32 %34)
  %36 = load %struct__Z17iteratorZ7vectori* %5
  %37 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %36)
  store %struct__Z17iteratorZ7vectori %37, %struct__Z17iteratorZ7vectori* %5
  %38 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %37, %struct__Z17iteratorZ7vectori* %38
  %asdf14 = getelementptr %struct__Z17iteratorZ7vectori* %38, i32 0, i32 0
  %asdf15 = getelementptr %struct__Z17iteratorZ7vectori* %38, i32 0, i32 1
  br label %_dale_internal_label_continuelabel_4

else11:                                           ; preds = %_dale_internal_label_continuelabel_4
  br label %_dale_internal_label_breaklabel_5

_dale_internal_label_breaklabel_5:                ; preds = %else11
  %39 = load %struct__Z7vectori** %9
  %40 = getelementptr %struct__Z7vectori* %39, i32 0, i32 1
  %41 = getelementptr %struct__Z17iteratorZ7vectori* %6, i32 0, i32 1
  %42 = load i32** %41
  store i32* %42, i32** %40
  %43 = load %struct__Z17iteratorZ7vectori* %1
  %asdf16 = getelementptr %struct__Z17iteratorZ7vectori* %6, i32 0, i32 0
  %asdf17 = getelementptr %struct__Z17iteratorZ7vectori* %6, i32 0, i32 1
  %asdf18 = getelementptr %struct__Z17iteratorZ7vectori* %5, i32 0, i32 0
  %asdf19 = getelementptr %struct__Z17iteratorZ7vectori* %5, i32 0, i32 1
  %asdf20 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 0
  %asdf21 = getelementptr %struct__Z17iteratorZ7vectori* %1, i32 0, i32 1
  ret %struct__Z17iteratorZ7vectori %43
}

define i1 @_Z5clearPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  br label %_dale_internal_label_continuelabel_6

_dale_internal_label_continuelabel_6:             ; preds = %then, %entry
  %1 = load %struct__Z7vectori** %0
  %2 = call i1 @_Z5emptyPZ7vectori(%struct__Z7vectori* %1)
  %3 = call i1 @_Z3notb(i1 %2)
  br i1 %3, label %then, label %else

then:                                             ; preds = %_dale_internal_label_continuelabel_6
  %4 = load %struct__Z7vectori** %0
  call void @"_Z8pop$2dbackPZ7vectori"(%struct__Z7vectori* %4)
  br label %_dale_internal_label_continuelabel_6

else:                                             ; preds = %_dale_internal_label_continuelabel_6
  br label %_dale_internal_label_breaklabel_7

_dale_internal_label_breaklabel_7:                ; preds = %else
  ret i1 true
}

define void @_Z4swapPZ7vectoriPZ7vectori(%struct__Z7vectori* %vec1, %struct__Z7vectori* %vec2) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vec1, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vec2, %struct__Z7vectori** %1
  %2 = alloca %struct__Z7vectori
  %3 = call i1 @_Z4initPZ7vectori(%struct__Z7vectori* %2)
  %4 = load %struct__Z7vectori** %0
  %5 = load %struct__Z7vectori* %4
  store %struct__Z7vectori %5, %struct__Z7vectori* %2
  %6 = alloca %struct__Z7vectori
  store %struct__Z7vectori %5, %struct__Z7vectori* %6
  %asdf = getelementptr %struct__Z7vectori* %6, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %6, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %6, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %6, i32 0, i32 3
  %7 = load %struct__Z7vectori** %0
  %8 = load %struct__Z7vectori** %1
  %9 = load %struct__Z7vectori* %8
  store %struct__Z7vectori %9, %struct__Z7vectori* %7
  %10 = alloca %struct__Z7vectori
  store %struct__Z7vectori %9, %struct__Z7vectori* %10
  %asdf4 = getelementptr %struct__Z7vectori* %10, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z7vectori* %10, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z7vectori* %10, i32 0, i32 2
  %asdf7 = getelementptr %struct__Z7vectori* %10, i32 0, i32 3
  %11 = load %struct__Z7vectori** %1
  %12 = load %struct__Z7vectori* %2
  store %struct__Z7vectori %12, %struct__Z7vectori* %11
  %13 = alloca %struct__Z7vectori
  store %struct__Z7vectori %12, %struct__Z7vectori* %13
  %asdf8 = getelementptr %struct__Z7vectori* %13, i32 0, i32 0
  %asdf9 = getelementptr %struct__Z7vectori* %13, i32 0, i32 1
  %asdf10 = getelementptr %struct__Z7vectori* %13, i32 0, i32 2
  %asdf11 = getelementptr %struct__Z7vectori* %13, i32 0, i32 3
  %asdf12 = getelementptr %struct__Z7vectori* %2, i32 0, i32 0
  %asdf13 = getelementptr %struct__Z7vectori* %2, i32 0, i32 1
  %asdf14 = getelementptr %struct__Z7vectori* %2, i32 0, i32 2
  %asdf15 = getelementptr %struct__Z7vectori* %2, i32 0, i32 3
  ret void
}

define i1 @"_Z9setf$2dcopyPZ7vectoriPZ7vectori"(%struct__Z7vectori* %dst, %struct__Z7vectori* %src) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %dst, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %src, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = call i1 @_Z4initPZ7vectori(%struct__Z7vectori* %2)
  %4 = load %struct__Z7vectori** %1
  %5 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %4)
  %6 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %5, %struct__Z17iteratorZ7vectori* %6
  %7 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %5, %struct__Z17iteratorZ7vectori* %7
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %7, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %7, i32 0, i32 1
  %8 = load %struct__Z7vectori** %1
  %9 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %8)
  %10 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %9, %struct__Z17iteratorZ7vectori* %10
  %11 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %9, %struct__Z17iteratorZ7vectori* %11
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %11, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %11, i32 0, i32 1
  br label %_dale_internal_label__8

_dale_internal_label__8:                          ; preds = %_dale_internal_label_continuelabel_9, %entry
  %12 = load %struct__Z17iteratorZ7vectori* %6
  %13 = load %struct__Z17iteratorZ7vectori* %10
  %14 = call i1 @"_Z2$21$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %12, %struct__Z17iteratorZ7vectori %13)
  br i1 %14, label %then, label %else

then:                                             ; preds = %_dale_internal_label__8
  %15 = load %struct__Z7vectori** %0
  %16 = load %struct__Z17iteratorZ7vectori* %6
  %17 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %16)
  %18 = call i1 @"_Z9push$2dbackPZ7vectorii"(%struct__Z7vectori* %15, i32 %17)
  br label %_dale_internal_label_continuelabel_9

else:                                             ; preds = %_dale_internal_label__8
  br label %_dale_internal_label_breaklabel_10

_dale_internal_label_continuelabel_9:             ; preds = %then
  %19 = load %struct__Z17iteratorZ7vectori* %6
  %20 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %19)
  store %struct__Z17iteratorZ7vectori %20, %struct__Z17iteratorZ7vectori* %6
  %21 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %20, %struct__Z17iteratorZ7vectori* %21
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %21, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %21, i32 0, i32 1
  br label %_dale_internal_label__8

_dale_internal_label_breaklabel_10:               ; preds = %else
  ret i1 true
}

define i1 @"_Z11setf$2dassignPZ7vectoriPZ7vectori"(%struct__Z7vectori* %dst, %struct__Z7vectori* %src) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %dst, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %src, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = call i1 @_Z5clearPZ7vectori(%struct__Z7vectori* %2)
  %4 = load %struct__Z7vectori** %0
  %5 = load %struct__Z7vectori** %1
  %6 = call i1 @"_Z9setf$2dcopyPZ7vectoriPZ7vectori"(%struct__Z7vectori* %4, %struct__Z7vectori* %5)
  ret i1 %6
}

define i32 @"_Z21_impl$2dof$2dconcept$2dTypeZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  %asdf = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i32 0
}

define i32 @"_Z27_impl$2dof$2dconcept$2dAssignableZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  %asdf = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i32 0
}

define i32 @"_Z26_impl$2dof$2dconcept$2dContainerZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  %asdf = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i32 0
}

define i32 @"_Z21_impl$2dof$2dconcept$2dTypePZ7vectori"(%struct__Z7vectori* %a) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %a, %struct__Z7vectori** %0
  ret i32 0
}

define i1 @"_Z1$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %c1, %struct__Z7vectori* %c2) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %c1, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %c2, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = call i32 @_Z4sizePZ7vectori(%struct__Z7vectori* %2)
  %4 = load %struct__Z7vectori** %1
  %5 = call i32 @_Z4sizePZ7vectori(%struct__Z7vectori* %4)
  %6 = call i1 @"_Z2$21$3dww"(i32 %3, i32 %5)
  br i1 %6, label %then, label %else

then:                                             ; preds = %entry
  ret i1 false

else:                                             ; preds = %entry
  br label %done_then_no_else

done_then_no_else:                                ; preds = %else
  %7 = load %struct__Z7vectori** %0
  %8 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %7)
  %9 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %8, %struct__Z17iteratorZ7vectori* %9
  %10 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %8, %struct__Z17iteratorZ7vectori* %10
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %10, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %10, i32 0, i32 1
  %11 = load %struct__Z7vectori** %1
  %12 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %11)
  %13 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %12, %struct__Z17iteratorZ7vectori* %13
  %14 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %12, %struct__Z17iteratorZ7vectori* %14
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %14, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %14, i32 0, i32 1
  %15 = load %struct__Z7vectori** %0
  %16 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %15)
  %17 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %16, %struct__Z17iteratorZ7vectori* %17
  %18 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %16, %struct__Z17iteratorZ7vectori* %18
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %18, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %18, i32 0, i32 1
  %19 = load %struct__Z7vectori** %1
  %20 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %19)
  %21 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %20, %struct__Z17iteratorZ7vectori* %21
  %22 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %20, %struct__Z17iteratorZ7vectori* %22
  %asdf6 = getelementptr %struct__Z17iteratorZ7vectori* %22, i32 0, i32 0
  %asdf7 = getelementptr %struct__Z17iteratorZ7vectori* %22, i32 0, i32 1
  br label %_dale_internal_label__11

_dale_internal_label__11:                         ; preds = %_dale_internal_label_continuelabel_12, %done_then_no_else
  %23 = load %struct__Z17iteratorZ7vectori* %9
  %24 = load %struct__Z17iteratorZ7vectori* %17
  %25 = call i1 @"_Z2$21$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %23, %struct__Z17iteratorZ7vectori %24)
  br i1 %25, label %then8, label %else9

then8:                                            ; preds = %_dale_internal_label__11
  %26 = load %struct__Z17iteratorZ7vectori* %9
  %27 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %26)
  %28 = load %struct__Z17iteratorZ7vectori* %13
  %29 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %28)
  %30 = call i1 @"_Z1$3dii"(i32 %27, i32 %29)
  %31 = call i1 @_Z3notb(i1 %30)
  br i1 %31, label %then10, label %else11

else9:                                            ; preds = %_dale_internal_label__11
  br label %_dale_internal_label_breaklabel_13

then10:                                           ; preds = %then8
  %asdf12 = getelementptr %struct__Z17iteratorZ7vectori* %21, i32 0, i32 0
  %asdf13 = getelementptr %struct__Z17iteratorZ7vectori* %21, i32 0, i32 1
  %asdf14 = getelementptr %struct__Z17iteratorZ7vectori* %17, i32 0, i32 0
  %asdf15 = getelementptr %struct__Z17iteratorZ7vectori* %17, i32 0, i32 1
  %asdf16 = getelementptr %struct__Z17iteratorZ7vectori* %13, i32 0, i32 0
  %asdf17 = getelementptr %struct__Z17iteratorZ7vectori* %13, i32 0, i32 1
  %asdf18 = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 0
  %asdf19 = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 1
  ret i1 false

else11:                                           ; preds = %then8
  br label %done_then_no_else20

done_then_no_else20:                              ; preds = %else11
  br label %_dale_internal_label_continuelabel_12

_dale_internal_label_continuelabel_12:            ; preds = %done_then_no_else20
  %32 = load %struct__Z17iteratorZ7vectori* %9
  %33 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %32)
  store %struct__Z17iteratorZ7vectori %33, %struct__Z17iteratorZ7vectori* %9
  %34 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %33, %struct__Z17iteratorZ7vectori* %34
  %asdf21 = getelementptr %struct__Z17iteratorZ7vectori* %34, i32 0, i32 0
  %asdf22 = getelementptr %struct__Z17iteratorZ7vectori* %34, i32 0, i32 1
  %35 = load %struct__Z17iteratorZ7vectori* %13
  %36 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %35)
  store %struct__Z17iteratorZ7vectori %36, %struct__Z17iteratorZ7vectori* %13
  %37 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %36, %struct__Z17iteratorZ7vectori* %37
  %asdf23 = getelementptr %struct__Z17iteratorZ7vectori* %37, i32 0, i32 0
  %asdf24 = getelementptr %struct__Z17iteratorZ7vectori* %37, i32 0, i32 1
  br label %_dale_internal_label__11

_dale_internal_label_breaklabel_13:               ; preds = %else9
  ret i1 true
}

define i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %c1, %struct__Z7vectori* %c2) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %c1, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %c2, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = call i32 @_Z4sizePZ7vectori(%struct__Z7vectori* %2)
  %4 = load %struct__Z7vectori** %1
  %5 = call i32 @_Z4sizePZ7vectori(%struct__Z7vectori* %4)
  %6 = call i1 @"_Z1$3cww"(i32 %3, i32 %5)
  br i1 %6, label %then, label %else

then:                                             ; preds = %entry
  ret i1 true

else:                                             ; preds = %entry
  br label %done_then_no_else

done_then_no_else:                                ; preds = %else
  %7 = load %struct__Z7vectori** %0
  %8 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %7)
  %9 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %8, %struct__Z17iteratorZ7vectori* %9
  %10 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %8, %struct__Z17iteratorZ7vectori* %10
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %10, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %10, i32 0, i32 1
  %11 = load %struct__Z7vectori** %1
  %12 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %11)
  %13 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %12, %struct__Z17iteratorZ7vectori* %13
  %14 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %12, %struct__Z17iteratorZ7vectori* %14
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %14, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %14, i32 0, i32 1
  %15 = load %struct__Z7vectori** %0
  %16 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %15)
  %17 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %16, %struct__Z17iteratorZ7vectori* %17
  %18 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %16, %struct__Z17iteratorZ7vectori* %18
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %18, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %18, i32 0, i32 1
  %19 = load %struct__Z7vectori** %1
  %20 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %19)
  %21 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %20, %struct__Z17iteratorZ7vectori* %21
  %22 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %20, %struct__Z17iteratorZ7vectori* %22
  %asdf6 = getelementptr %struct__Z17iteratorZ7vectori* %22, i32 0, i32 0
  %asdf7 = getelementptr %struct__Z17iteratorZ7vectori* %22, i32 0, i32 1
  br label %_dale_internal_label__14

_dale_internal_label__14:                         ; preds = %_dale_internal_label_continuelabel_15, %done_then_no_else
  %23 = load %struct__Z17iteratorZ7vectori* %9
  %24 = load %struct__Z17iteratorZ7vectori* %17
  %25 = call i1 @"_Z2$21$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %23, %struct__Z17iteratorZ7vectori %24)
  br i1 %25, label %then8, label %else9

then8:                                            ; preds = %_dale_internal_label__14
  %26 = load %struct__Z17iteratorZ7vectori* %9
  %27 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %26)
  %28 = alloca i32
  store i32 %27, i32* %28
  %29 = load %struct__Z17iteratorZ7vectori* %13
  %30 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %29)
  %31 = alloca i32
  store i32 %30, i32* %31
  %32 = load i32* %28
  %33 = load i32* %31
  %34 = call i1 @"_Z2$21$3dii"(i32 %32, i32 %33)
  br i1 %34, label %then10, label %else11

else9:                                            ; preds = %_dale_internal_label__14
  br label %_dale_internal_label_breaklabel_16

then10:                                           ; preds = %then8
  %35 = load i32* %28
  %36 = load i32* %31
  %37 = call i1 @"_Z1$3cii"(i32 %35, i32 %36)
  %asdf12 = getelementptr %struct__Z17iteratorZ7vectori* %21, i32 0, i32 0
  %asdf13 = getelementptr %struct__Z17iteratorZ7vectori* %21, i32 0, i32 1
  %asdf14 = getelementptr %struct__Z17iteratorZ7vectori* %17, i32 0, i32 0
  %asdf15 = getelementptr %struct__Z17iteratorZ7vectori* %17, i32 0, i32 1
  %asdf16 = getelementptr %struct__Z17iteratorZ7vectori* %13, i32 0, i32 0
  %asdf17 = getelementptr %struct__Z17iteratorZ7vectori* %13, i32 0, i32 1
  %asdf18 = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 0
  %asdf19 = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 1
  ret i1 %37

else11:                                           ; preds = %then8
  br label %done_then_no_else20

done_then_no_else20:                              ; preds = %else11
  br label %_dale_internal_label_continuelabel_15

_dale_internal_label_continuelabel_15:            ; preds = %done_then_no_else20
  %38 = load %struct__Z17iteratorZ7vectori* %9
  %39 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %38)
  store %struct__Z17iteratorZ7vectori %39, %struct__Z17iteratorZ7vectori* %9
  %40 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %39, %struct__Z17iteratorZ7vectori* %40
  %asdf21 = getelementptr %struct__Z17iteratorZ7vectori* %40, i32 0, i32 0
  %asdf22 = getelementptr %struct__Z17iteratorZ7vectori* %40, i32 0, i32 1
  %41 = load %struct__Z17iteratorZ7vectori* %13
  %42 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %41)
  store %struct__Z17iteratorZ7vectori %42, %struct__Z17iteratorZ7vectori* %13
  %43 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %42, %struct__Z17iteratorZ7vectori* %43
  %asdf23 = getelementptr %struct__Z17iteratorZ7vectori* %43, i32 0, i32 0
  %asdf24 = getelementptr %struct__Z17iteratorZ7vectori* %43, i32 0, i32 1
  br label %_dale_internal_label__14

_dale_internal_label_breaklabel_16:               ; preds = %else9
  ret i1 true
}

define i1 @"_Z2$21$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %a, %struct__Z7vectori* %b) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %a, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %b, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = load %struct__Z7vectori** %1
  %4 = call i1 @"_Z1$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %2, %struct__Z7vectori* %3)
  %5 = call i1 @_Z3notb(i1 %4)
  ret i1 %5
}

define i1 @"_Z2$3c$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %a, %struct__Z7vectori* %b) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %a, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %b, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = load %struct__Z7vectori** %1
  %4 = call i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %2, %struct__Z7vectori* %3)
  br i1 %4, label %then, label %else

then:                                             ; preds = %entry
  br label %done_phi

else:                                             ; preds = %entry
  %5 = load %struct__Z7vectori** %1
  %6 = load %struct__Z7vectori** %0
  %7 = call i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %5, %struct__Z7vectori* %6)
  %8 = call i1 @_Z3notb(i1 %7)
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ true, %then ], [ %8, %else ]
  ret i1 %9
}

define i1 @"_Z1$3ePZ7vectoriPZ7vectori"(%struct__Z7vectori* %a, %struct__Z7vectori* %b) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %a, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %b, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = load %struct__Z7vectori** %1
  %4 = call i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %2, %struct__Z7vectori* %3)
  %5 = call i1 @_Z3notb(i1 %4)
  br i1 %5, label %then, label %else

then:                                             ; preds = %entry
  %6 = load %struct__Z7vectori** %1
  %7 = load %struct__Z7vectori** %0
  %8 = call i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %6, %struct__Z7vectori* %7)
  br label %done_phi

else:                                             ; preds = %entry
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ %8, %then ], [ false, %else ]
  ret i1 %9
}

define i1 @"_Z2$3e$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %a, %struct__Z7vectori* %b) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %a, %struct__Z7vectori** %0
  %1 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %b, %struct__Z7vectori** %1
  %2 = load %struct__Z7vectori** %0
  %3 = load %struct__Z7vectori** %1
  %4 = call i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %2, %struct__Z7vectori* %3)
  %5 = call i1 @_Z3notb(i1 %4)
  br i1 %5, label %then, label %else

then:                                             ; preds = %entry
  br label %done_phi

else:                                             ; preds = %entry
  %6 = load %struct__Z7vectori** %1
  %7 = load %struct__Z7vectori** %0
  %8 = call i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %6, %struct__Z7vectori* %7)
  br label %done_phi

done_phi:                                         ; preds = %else, %then
  %9 = phi i1 [ true, %then ], [ %8, %else ]
  ret i1 %9
}

define i1 @"_Z1$3dZ7vectoriZ7vectori"(%struct__Z7vectori %fst, %struct__Z7vectori %snd) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %fst, %struct__Z7vectori* %0
  %1 = alloca %struct__Z7vectori
  store %struct__Z7vectori %snd, %struct__Z7vectori* %1
  %2 = call i1 @"_Z1$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %0, %struct__Z7vectori* %1)
  %asdf = getelementptr %struct__Z7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %1, i32 0, i32 3
  %asdf4 = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf7 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i1 %2
}

define i1 @"_Z2$21$3dZ7vectoriZ7vectori"(%struct__Z7vectori %fst, %struct__Z7vectori %snd) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %fst, %struct__Z7vectori* %0
  %1 = alloca %struct__Z7vectori
  store %struct__Z7vectori %snd, %struct__Z7vectori* %1
  %2 = call i1 @"_Z2$21$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %0, %struct__Z7vectori* %1)
  %asdf = getelementptr %struct__Z7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %1, i32 0, i32 3
  %asdf4 = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf7 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i1 %2
}

define i1 @"_Z1$3cZ7vectoriZ7vectori"(%struct__Z7vectori %fst, %struct__Z7vectori %snd) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %fst, %struct__Z7vectori* %0
  %1 = alloca %struct__Z7vectori
  store %struct__Z7vectori %snd, %struct__Z7vectori* %1
  %2 = call i1 @"_Z1$3cPZ7vectoriPZ7vectori"(%struct__Z7vectori* %0, %struct__Z7vectori* %1)
  %asdf = getelementptr %struct__Z7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %1, i32 0, i32 3
  %asdf4 = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf7 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i1 %2
}

define i1 @"_Z1$3eZ7vectoriZ7vectori"(%struct__Z7vectori %fst, %struct__Z7vectori %snd) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %fst, %struct__Z7vectori* %0
  %1 = alloca %struct__Z7vectori
  store %struct__Z7vectori %snd, %struct__Z7vectori* %1
  %2 = call i1 @"_Z1$3ePZ7vectoriPZ7vectori"(%struct__Z7vectori* %0, %struct__Z7vectori* %1)
  %asdf = getelementptr %struct__Z7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %1, i32 0, i32 3
  %asdf4 = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf7 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i1 %2
}

define i1 @"_Z2$3c$3dZ7vectoriZ7vectori"(%struct__Z7vectori %fst, %struct__Z7vectori %snd) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %fst, %struct__Z7vectori* %0
  %1 = alloca %struct__Z7vectori
  store %struct__Z7vectori %snd, %struct__Z7vectori* %1
  %2 = call i1 @"_Z2$3c$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %0, %struct__Z7vectori* %1)
  %asdf = getelementptr %struct__Z7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %1, i32 0, i32 3
  %asdf4 = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf7 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i1 %2
}

define i1 @"_Z2$3e$3dZ7vectoriZ7vectori"(%struct__Z7vectori %fst, %struct__Z7vectori %snd) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %fst, %struct__Z7vectori* %0
  %1 = alloca %struct__Z7vectori
  store %struct__Z7vectori %snd, %struct__Z7vectori* %1
  %2 = call i1 @"_Z2$3e$3dPZ7vectoriPZ7vectori"(%struct__Z7vectori* %0, %struct__Z7vectori* %1)
  %asdf = getelementptr %struct__Z7vectori* %1, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %1, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %1, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %1, i32 0, i32 3
  %asdf4 = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf7 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i1 %2
}

define i32 @"_Z35_impl$2dof$2dconcept$2dEqualityComparableZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  %asdf = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i32 0
}

define i32 @"_Z35_impl$2dof$2dconcept$2dLessThanComparableZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  %asdf = getelementptr %struct__Z7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z7vectori* %0, i32 0, i32 1
  %asdf2 = getelementptr %struct__Z7vectori* %0, i32 0, i32 2
  %asdf3 = getelementptr %struct__Z7vectori* %0, i32 0, i32 3
  ret i32 0
}

define void @_Z7destroyPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = call i1 @_Z5clearPZ7vectori(%struct__Z7vectori* %1)
  ret void
}

define void @_Z4swapPZ17iteratorZ7vectoriPZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori* %a, %struct__Z17iteratorZ7vectori* %b) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori*
  store %struct__Z17iteratorZ7vectori* %a, %struct__Z17iteratorZ7vectori** %0
  %1 = alloca %struct__Z17iteratorZ7vectori*
  store %struct__Z17iteratorZ7vectori* %b, %struct__Z17iteratorZ7vectori** %1
  %2 = alloca %struct__Z17iteratorZ7vectori
  %3 = load %struct__Z17iteratorZ7vectori** %0
  %4 = load %struct__Z17iteratorZ7vectori* %3
  store %struct__Z17iteratorZ7vectori %4, %struct__Z17iteratorZ7vectori* %2
  %5 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %4, %struct__Z17iteratorZ7vectori* %5
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %5, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %5, i32 0, i32 1
  %6 = load %struct__Z17iteratorZ7vectori** %0
  %7 = load %struct__Z17iteratorZ7vectori** %1
  %8 = load %struct__Z17iteratorZ7vectori* %7
  store %struct__Z17iteratorZ7vectori %8, %struct__Z17iteratorZ7vectori* %6
  %9 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %8, %struct__Z17iteratorZ7vectori* %9
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 1
  %10 = load %struct__Z17iteratorZ7vectori** %1
  %11 = load %struct__Z17iteratorZ7vectori* %2
  store %struct__Z17iteratorZ7vectori %11, %struct__Z17iteratorZ7vectori* %10
  %12 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %11, %struct__Z17iteratorZ7vectori* %12
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %12, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %12, i32 0, i32 1
  %asdf6 = getelementptr %struct__Z17iteratorZ7vectori* %2, i32 0, i32 0
  %asdf7 = getelementptr %struct__Z17iteratorZ7vectori* %2, i32 0, i32 1
  ret void
}

define void @_Z4swapPZ18riteratorZ7vectoriPZ18riteratorZ7vectori(%struct__Z18riteratorZ7vectori* %a, %struct__Z18riteratorZ7vectori* %b) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori*
  store %struct__Z18riteratorZ7vectori* %a, %struct__Z18riteratorZ7vectori** %0
  %1 = alloca %struct__Z18riteratorZ7vectori*
  store %struct__Z18riteratorZ7vectori* %b, %struct__Z18riteratorZ7vectori** %1
  %2 = alloca %struct__Z18riteratorZ7vectori
  %3 = load %struct__Z18riteratorZ7vectori** %0
  %4 = load %struct__Z18riteratorZ7vectori* %3
  store %struct__Z18riteratorZ7vectori %4, %struct__Z18riteratorZ7vectori* %2
  %5 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %4, %struct__Z18riteratorZ7vectori* %5
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %5, i32 0, i32 0
  %6 = load %struct__Z18riteratorZ7vectori** %0
  %7 = load %struct__Z18riteratorZ7vectori** %1
  %8 = load %struct__Z18riteratorZ7vectori* %7
  store %struct__Z18riteratorZ7vectori %8, %struct__Z18riteratorZ7vectori* %6
  %9 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %8, %struct__Z18riteratorZ7vectori* %9
  %asdf1 = getelementptr %struct__Z18riteratorZ7vectori* %9, i32 0, i32 0
  %10 = load %struct__Z18riteratorZ7vectori** %1
  %11 = load %struct__Z18riteratorZ7vectori* %2
  store %struct__Z18riteratorZ7vectori %11, %struct__Z18riteratorZ7vectori* %10
  %12 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %11, %struct__Z18riteratorZ7vectori* %12
  %asdf2 = getelementptr %struct__Z18riteratorZ7vectori* %12, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z18riteratorZ7vectori* %2, i32 0, i32 0
  ret void
}

define i32 @"_Z27_impl$2dof$2dconcept$2dAssignableZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i32 @"_Z31_impl$2dof$2dconcept$2dOutputIteratorZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i32 @"_Z27_impl$2dof$2dconcept$2dAssignableZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i32 @"_Z31_impl$2dof$2dconcept$2dOutputIteratorZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i32 @"_Z32_impl$2dof$2dconcept$2dTrivialIteratorZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i32 @"_Z30_impl$2dof$2dconcept$2dInputIteratorZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i32 @"_Z32_impl$2dof$2dconcept$2dForwardIteratorZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i32 @"_Z38_impl$2dof$2dconcept$2dBidirectionalIteratorZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i32 @"_Z37_impl$2dof$2dconcept$2dRandomAccessIteratorZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %a, %struct__Z17iteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %0, i32 0, i32 1
  ret i32 0
}

define i32 @"_Z32_impl$2dof$2dconcept$2dTrivialIteratorZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i32 @"_Z30_impl$2dof$2dconcept$2dInputIteratorZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i32 @"_Z32_impl$2dof$2dconcept$2dForwardIteratorZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i32 @"_Z38_impl$2dof$2dconcept$2dBidirectionalIteratorZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i32 @"_Z37_impl$2dof$2dconcept$2dRandomAccessIteratorZ18riteratorZ7vectori"(%struct__Z18riteratorZ7vectori %a) {
entry:
  %0 = alloca %struct__Z18riteratorZ7vectori
  store %struct__Z18riteratorZ7vectori %a, %struct__Z18riteratorZ7vectori* %0
  %asdf = getelementptr %struct__Z18riteratorZ7vectori* %0, i32 0, i32 0
  ret i32 0
}

define i32 @"_Z33_impl$2dof$2dconcept$2dForwardContainerZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  call void @_Z7destroyPZ7vectori(%struct__Z7vectori* %0)
  ret i32 0
}

define i32 @"_Z25_impl$2dof$2dconcept$2dSequenceZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  call void @_Z7destroyPZ7vectori(%struct__Z7vectori* %0)
  ret i32 0
}

define i32 @"_Z38_impl$2dof$2dconcept$2dBackInsertionSequenceZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  call void @_Z7destroyPZ7vectori(%struct__Z7vectori* %0)
  ret i32 0
}

define i32 @"_Z36_impl$2dof$2dconcept$2dReversibleContainerZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  call void @_Z7destroyPZ7vectori(%struct__Z7vectori* %0)
  ret i32 0
}

define i32 @"_Z38_impl$2dof$2dconcept$2dRandomAccessContainerZ7vectori"(%struct__Z7vectori %a) {
entry:
  %0 = alloca %struct__Z7vectori
  store %struct__Z7vectori %a, %struct__Z7vectori* %0
  call void @_Z7destroyPZ7vectori(%struct__Z7vectori* %0)
  ret i32 0
}

define internal void @_Z6printvPZ7vectori(%struct__Z7vectori* %vecp) {
entry:
  %0 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %vecp, %struct__Z7vectori** %0
  %1 = load %struct__Z7vectori** %0
  %2 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %1)
  %3 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %2, %struct__Z17iteratorZ7vectori* %3
  %4 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %2, %struct__Z17iteratorZ7vectori* %4
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %4, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %4, i32 0, i32 1
  %5 = load %struct__Z7vectori** %0
  %6 = call %struct__Z17iteratorZ7vectori @_Z3endPZ7vectori(%struct__Z7vectori* %5)
  %7 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %6, %struct__Z17iteratorZ7vectori* %7
  %8 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %6, %struct__Z17iteratorZ7vectori* %8
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %8, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %8, i32 0, i32 1
  br label %_dale_internal_label__17

_dale_internal_label__17:                         ; preds = %_dale_internal_label_continuelabel_18, %entry
  %9 = load %struct__Z17iteratorZ7vectori* %3
  %10 = load %struct__Z17iteratorZ7vectori* %7
  %11 = call i1 @"_Z2$21$3dZ17iteratorZ7vectoriZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %9, %struct__Z17iteratorZ7vectori %10)
  br i1 %11, label %then, label %else

then:                                             ; preds = %_dale_internal_label__17
  %12 = load %struct__Z17iteratorZ7vectori* %3
  %13 = call i32 @"_Z7$40sourceZ17iteratorZ7vectori"(%struct__Z17iteratorZ7vectori %12)
  %14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @_dv162, i32 0, i32 0), i32 %13)
  br label %_dale_internal_label_continuelabel_18

else:                                             ; preds = %_dale_internal_label__17
  br label %_dale_internal_label_breaklabel_19

_dale_internal_label_continuelabel_18:            ; preds = %then
  %15 = load %struct__Z17iteratorZ7vectori* %3
  %16 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %15)
  store %struct__Z17iteratorZ7vectori %16, %struct__Z17iteratorZ7vectori* %3
  %17 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %16, %struct__Z17iteratorZ7vectori* %17
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %17, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %17, i32 0, i32 1
  %18 = load [4 x i8]* @_dv162
  br label %_dale_internal_label__17

_dale_internal_label_breaklabel_19:               ; preds = %else
  ret void
}

define i32 @main() {
entry:
  %0 = call i32 @"_Z13init$2dchannels"()
  %1 = alloca %struct__Z7vectori
  %2 = call i1 @_Z4initPZ7vectorii(%struct__Z7vectori* %1, i32 10)
  %3 = alloca %struct__Z7vectori*
  store %struct__Z7vectori* %1, %struct__Z7vectori** %3
  %4 = load %struct__Z7vectori** %3
  call void @_Z6printvPZ7vectori(%struct__Z7vectori* %4)
  %5 = load %struct__Z7vectori** %3
  %6 = load %struct__Z7vectori** %3
  %7 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %6)
  %8 = call %struct__Z17iteratorZ7vectori @_Z6insertPZ7vectoriZ17iteratorZ7vectorii(%struct__Z7vectori* %5, %struct__Z17iteratorZ7vectori %7, i32 100)
  %9 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %8, %struct__Z17iteratorZ7vectori* %9
  %asdf = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 0
  %asdf1 = getelementptr %struct__Z17iteratorZ7vectori* %9, i32 0, i32 1
  %10 = load %struct__Z7vectori** %3
  %11 = load %struct__Z7vectori** %3
  %12 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %11)
  %13 = call %struct__Z17iteratorZ7vectori @_Z6insertPZ7vectoriZ17iteratorZ7vectorii(%struct__Z7vectori* %10, %struct__Z17iteratorZ7vectori %12, i32 200)
  %14 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %13, %struct__Z17iteratorZ7vectori* %14
  %asdf2 = getelementptr %struct__Z17iteratorZ7vectori* %14, i32 0, i32 0
  %asdf3 = getelementptr %struct__Z17iteratorZ7vectori* %14, i32 0, i32 1
  %15 = load %struct__Z7vectori** %3
  %16 = load %struct__Z7vectori** %3
  %17 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %16)
  %18 = call %struct__Z17iteratorZ7vectori @_Z6insertPZ7vectoriZ17iteratorZ7vectorii(%struct__Z7vectori* %15, %struct__Z17iteratorZ7vectori %17, i32 300)
  %19 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %18, %struct__Z17iteratorZ7vectori* %19
  %asdf4 = getelementptr %struct__Z17iteratorZ7vectori* %19, i32 0, i32 0
  %asdf5 = getelementptr %struct__Z17iteratorZ7vectori* %19, i32 0, i32 1
  %20 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([16 x i8]* @_dv163, i32 0, i32 0))
  %21 = load %struct__Z7vectori** %3
  call void @_Z6printvPZ7vectori(%struct__Z7vectori* %21)
  %22 = load %struct__Z7vectori** %3
  %23 = load %struct__Z7vectori** %3
  %24 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %23)
  %25 = call %struct__Z17iteratorZ7vectori @_Z9successorZ17iteratorZ7vectori(%struct__Z17iteratorZ7vectori %24)
  %26 = call %struct__Z17iteratorZ7vectori @_Z5erasePZ7vectoriZ17iteratorZ7vectori(%struct__Z7vectori* %22, %struct__Z17iteratorZ7vectori %25)
  %27 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %26, %struct__Z17iteratorZ7vectori* %27
  %asdf6 = getelementptr %struct__Z17iteratorZ7vectori* %27, i32 0, i32 0
  %asdf7 = getelementptr %struct__Z17iteratorZ7vectori* %27, i32 0, i32 1
  %28 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([25 x i8]* @_dv164, i32 0, i32 0))
  %29 = load %struct__Z7vectori** %3
  call void @_Z6printvPZ7vectori(%struct__Z7vectori* %29)
  %30 = load %struct__Z7vectori** %3
  %31 = load %struct__Z7vectori** %3
  %32 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %31)
  %33 = call %struct__Z17iteratorZ7vectori @_Z5erasePZ7vectoriZ17iteratorZ7vectori(%struct__Z7vectori* %30, %struct__Z17iteratorZ7vectori %32)
  %34 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %33, %struct__Z17iteratorZ7vectori* %34
  %asdf8 = getelementptr %struct__Z17iteratorZ7vectori* %34, i32 0, i32 0
  %asdf9 = getelementptr %struct__Z17iteratorZ7vectori* %34, i32 0, i32 1
  %35 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([15 x i8]* @_dv165, i32 0, i32 0))
  %36 = load %struct__Z7vectori** %3
  call void @_Z6printvPZ7vectori(%struct__Z7vectori* %36)
  %37 = load %struct__Z7vectori** %3
  %38 = load %struct__Z7vectori** %3
  %39 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %38)
  %40 = call %struct__Z17iteratorZ7vectori @_Z5erasePZ7vectoriZ17iteratorZ7vectori(%struct__Z7vectori* %37, %struct__Z17iteratorZ7vectori %39)
  %41 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %40, %struct__Z17iteratorZ7vectori* %41
  %asdf10 = getelementptr %struct__Z17iteratorZ7vectori* %41, i32 0, i32 0
  %asdf11 = getelementptr %struct__Z17iteratorZ7vectori* %41, i32 0, i32 1
  %42 = load %struct__Z7vectori** %3
  %43 = load %struct__Z7vectori** %3
  %44 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %43)
  %45 = call %struct__Z17iteratorZ7vectori @_Z5erasePZ7vectoriZ17iteratorZ7vectori(%struct__Z7vectori* %42, %struct__Z17iteratorZ7vectori %44)
  %46 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %45, %struct__Z17iteratorZ7vectori* %46
  %asdf12 = getelementptr %struct__Z17iteratorZ7vectori* %46, i32 0, i32 0
  %asdf13 = getelementptr %struct__Z17iteratorZ7vectori* %46, i32 0, i32 1
  %47 = load %struct__Z7vectori** %3
  %48 = load %struct__Z7vectori** %3
  %49 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %48)
  %50 = call %struct__Z17iteratorZ7vectori @_Z5erasePZ7vectoriZ17iteratorZ7vectori(%struct__Z7vectori* %47, %struct__Z17iteratorZ7vectori %49)
  %51 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %50, %struct__Z17iteratorZ7vectori* %51
  %asdf14 = getelementptr %struct__Z17iteratorZ7vectori* %51, i32 0, i32 0
  %asdf15 = getelementptr %struct__Z17iteratorZ7vectori* %51, i32 0, i32 1
  %52 = load %struct__Z7vectori** %3
  %53 = load %struct__Z7vectori** %3
  %54 = call %struct__Z17iteratorZ7vectori @_Z5beginPZ7vectori(%struct__Z7vectori* %53)
  %55 = call %struct__Z17iteratorZ7vectori @_Z5erasePZ7vectoriZ17iteratorZ7vectori(%struct__Z7vectori* %52, %struct__Z17iteratorZ7vectori %54)
  %56 = alloca %struct__Z17iteratorZ7vectori
  store %struct__Z17iteratorZ7vectori %55, %struct__Z17iteratorZ7vectori* %56
  %asdf16 = getelementptr %struct__Z17iteratorZ7vectori* %56, i32 0, i32 0
  %asdf17 = getelementptr %struct__Z17iteratorZ7vectori* %56, i32 0, i32 1
  %57 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([13 x i8]* @_dv166, i32 0, i32 0))
  %58 = load %struct__Z7vectori** %3
  call void @_Z6printvPZ7vectori(%struct__Z7vectori* %58)
  call void @_Z7destroyPZ7vectori(%struct__Z7vectori* %1)
  ret i32 0
}
