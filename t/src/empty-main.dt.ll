; ModuleID = '../dale/t/src/empty-main.dt'

%struct__Z3_vp = type { i8 }
%struct__Z5DNode = type { i32, i8*, %struct__Z5DNode*, %struct__Z5DNode*, i32, i32, i32, i32, i32, i32, i32, i32, i8* }
%"struct__Z7va$2dlist" = type { i32, i32, i8*, i8* }
%struct__Z8PoolNode = type { %struct__Z3_vp*, %struct__Z8PoolNode*, %struct__Z8PoolNode* }

@JMP_BUF_SIZE = linkonce_odr global i32 200
@FPOS_T = linkonce_odr global i32 16
@TIME_T = linkonce_odr global i32 8
@CLOCK_T = linkonce_odr global i32 8
@SIZEOF_LONG = linkonce_odr global i32 8
@SIZEOF_LONG_LONG = linkonce_odr global i32 8
@FLT_RADIX = linkonce_odr global i32 2
@FLT_MANT_DIG = linkonce_odr global i32 24
@FLT_DIG = linkonce_odr global i32 6
@FLT_EPSILON = linkonce_odr global float 0x3E80000000000000
@FLT_ROUNDS = linkonce_odr global i32 1
@FLT_MIN_EXP = linkonce_odr global i32 -125
@FLT_MAX_EXP = linkonce_odr global i32 128
@FLT_MIN = linkonce_odr global float 0x3810000000000000
@FLT_MAX = linkonce_odr global float 0x47EFFFFFE0000000
@DBL_MANT_DIG = linkonce_odr global i32 53
@DBL_DIG = linkonce_odr global i32 15
@DBL_EPSILON = linkonce_odr global double 0x3CB0000000000000
@DBL_MIN_EXP = linkonce_odr global i32 -1021
@DBL_MAX_EXP = linkonce_odr global i32 1024
@DBL_MIN = linkonce_odr global double 0x10000000000000
@DBL_MAX = linkonce_odr global double 0x7FEFFFFFFFFFFFFF
@LDBL_MANT_DIG = linkonce_odr global i32 64
@LDBL_DIG = linkonce_odr global i32 18
@LDBL_EPSILON = linkonce_odr global x86_fp80 0xK3FC08000000000000000
@LDBL_MIN_EXP = linkonce_odr global i32 -16381
@LDBL_MAX_EXP = linkonce_odr global i32 16384
@LDBL_MIN = linkonce_odr global x86_fp80 0xK00000000000000000000
@LDBL_MAX = linkonce_odr global x86_fp80 0xK7FFF8000000000000000
@HUGE_VAL = linkonce_odr global double 0x7FF0000000000000
@HUGE_VALF = linkonce_odr global float 0x7FF0000000000000
@HUGE_VALL = linkonce_odr global x86_fp80 0xK7FFF8000000000000000
@L_tmpnam = linkonce_odr global i32 20
@TMP_MAX = linkonce_odr global i32 238328
@FILENAME_MAX = linkonce_odr global i32 4096
@FOPEN_MAX = linkonce_odr global i32 16
@RAND_MAX = linkonce_odr global i32 2147483647
@EXIT_FAILURE = linkonce_odr global i32 1
@EXIT_SUCCESS = linkonce_odr global i32 0

define linkonce i32 @"_Z1$2bii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$2dii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2fii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2aii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i32 %b, %a
  ret i32 %0
}

define linkonce i1 @"_Z1$3dii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp slt i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sle i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3eii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sgt i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sge i32 %a, %b
  ret i1 %0
}

define linkonce i32 @"_Z2$3c$3cii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = shl i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z2$3e$3eii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = lshr i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$26ii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$7cii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$5eii"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i32 %b, %a
  ret i32 %0
}

define linkonce i8 @"_Z1$2bcc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$2dcc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i8 %a, %b
  ret i8 %0
}

define linkonce i8 @"_Z1$2fcc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i8 %a, %b
  ret i8 %0
}

define linkonce i8 @"_Z1$2acc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i8 %b, %a
  ret i8 %0
}

define linkonce i1 @"_Z1$3dcc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dcc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3ccc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp slt i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dcc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sle i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3ecc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sgt i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dcc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sge i8 %a, %b
  ret i1 %0
}

define linkonce i8 @"_Z2$3c$3cci"(i8 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i8
  %1 = shl i8 %a, %0
  ret i8 %1
}

define linkonce i8 @"_Z2$3e$3eci"(i8 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i8
  %1 = lshr i8 %a, %0
  ret i8 %1
}

define linkonce i8 @"_Z1$26cc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$7ccc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$5ecc"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i8 %b, %a
  ret i8 %0
}

define linkonce i32 @"_Z1$2bjj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$2djj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2fjj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2ajj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i32 %b, %a
  ret i32 %0
}

define linkonce i1 @"_Z1$3djj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3djj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cjj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3djj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3ejj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3djj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i32 %a, %b
  ret i1 %0
}

define linkonce i32 @"_Z2$3c$3cji"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = shl i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z2$3e$3eji"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = lshr i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$26jj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$7cjj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$5ejj"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$7ej"(i32 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i32 %a, -1
  ret i32 %0
}

define linkonce i64 @"_Z1$2bll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$2dll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2fll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2all"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i64 %b, %a
  ret i64 %0
}

define linkonce i1 @"_Z1$3dll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3ell"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i64 %a, %b
  ret i1 %0
}

define linkonce i64 @"_Z2$3c$3cli"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = shl i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z2$3e$3eli"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = lshr i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z1$26ll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7cll"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$5ell"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7el"(i64 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %a, -1
  ret i64 %0
}

define linkonce i64 @"_Z1$2bxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$2dxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2fxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2axx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i64 %b, %a
  ret i64 %0
}

define linkonce i1 @"_Z1$3dxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3exx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i64 %a, %b
  ret i1 %0
}

define linkonce i64 @"_Z2$3c$3cxi"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = shl i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z2$3e$3exi"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = lshr i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z1$26xx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7cxx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$5exx"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7ex"(i64 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %a, -1
  ret i64 %0
}

define linkonce i64 @"_Z1$2bww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$2dww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2fww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2aww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i64 %b, %a
  ret i64 %0
}

define linkonce i1 @"_Z1$3dww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3eww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i64 %a, %b
  ret i1 %0
}

define linkonce i64 @"_Z2$3c$3cwi"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = shl i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z2$3e$3ewi"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = lshr i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z1$26ww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7cww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$5eww"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7ew"(i64 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %a, -1
  ret i64 %0
}

define linkonce i8 @"_Z1$2byy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$2dyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i8 %a, %b
  ret i8 %0
}

define linkonce i8 @"_Z1$2fyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i8 %a, %b
  ret i8 %0
}

define linkonce i8 @"_Z1$2ayy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i8 %b, %a
  ret i8 %0
}

define linkonce i1 @"_Z1$3dyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp slt i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sle i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3eyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sgt i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sge i8 %a, %b
  ret i1 %0
}

define linkonce i8 @"_Z2$3c$3cyi"(i8 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i8
  %1 = shl i8 %a, %0
  ret i8 %1
}

define linkonce i8 @"_Z2$3e$3eyi"(i8 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i8
  %1 = lshr i8 %a, %0
  ret i8 %1
}

define linkonce i8 @"_Z1$26yy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$7cyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$5eyy"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$2bmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$2dmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i8 %a, %b
  ret i8 %0
}

define linkonce i8 @"_Z1$2fmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i8 %a, %b
  ret i8 %0
}

define linkonce i8 @"_Z1$2amm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i8 %b, %a
  ret i8 %0
}

define linkonce i1 @"_Z1$3dmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3emm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i8 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i8 %a, %b
  ret i1 %0
}

define linkonce i8 @"_Z2$3c$3cmi"(i8 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i8
  %1 = shl i8 %a, %0
  ret i8 %1
}

define linkonce i8 @"_Z2$3e$3emi"(i8 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i8
  %1 = lshr i8 %a, %0
  ret i8 %1
}

define linkonce i8 @"_Z1$26mm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$7cmm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$5emm"(i8 %a, i8 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i8 %b, %a
  ret i8 %0
}

define linkonce i8 @"_Z1$7em"(i8 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i8 %a, -1
  ret i8 %0
}

define linkonce i16 @"_Z1$2bnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$2dnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i16 %a, %b
  ret i16 %0
}

define linkonce i16 @"_Z1$2fnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i16 %a, %b
  ret i16 %0
}

define linkonce i16 @"_Z1$2ann"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i16 %b, %a
  ret i16 %0
}

define linkonce i1 @"_Z1$3dnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp slt i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sle i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3enn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sgt i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sge i16 %a, %b
  ret i1 %0
}

define linkonce i16 @"_Z2$3c$3cni"(i16 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i16
  %1 = shl i16 %a, %0
  ret i16 %1
}

define linkonce i16 @"_Z2$3e$3eni"(i16 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i16
  %1 = lshr i16 %a, %0
  ret i16 %1
}

define linkonce i16 @"_Z1$26nn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$7cnn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$5enn"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$2boo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$2doo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i16 %a, %b
  ret i16 %0
}

define linkonce i16 @"_Z1$2foo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i16 %a, %b
  ret i16 %0
}

define linkonce i16 @"_Z1$2aoo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i16 %b, %a
  ret i16 %0
}

define linkonce i1 @"_Z1$3doo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3doo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3coo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3doo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3eoo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i16 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3doo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i16 %a, %b
  ret i1 %0
}

define linkonce i16 @"_Z2$3c$3coi"(i16 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i16
  %1 = shl i16 %a, %0
  ret i16 %1
}

define linkonce i16 @"_Z2$3e$3eoi"(i16 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = trunc i32 %b to i16
  %1 = lshr i16 %a, %0
  ret i16 %1
}

define linkonce i16 @"_Z1$26oo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$7coo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$5eoo"(i16 %a, i16 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i16 %b, %a
  ret i16 %0
}

define linkonce i16 @"_Z1$7eo"(i16 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i16 %a, -1
  ret i16 %0
}

define linkonce i32 @"_Z1$2bpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$2dpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2fpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2app"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i32 %b, %a
  ret i32 %0
}

define linkonce i1 @"_Z1$3dpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp slt i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sle i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3epp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sgt i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sge i32 %a, %b
  ret i1 %0
}

define linkonce i32 @"_Z2$3c$3cpi"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = shl i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z2$3e$3epi"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = lshr i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$26pp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$7cpp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$5epp"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$2bqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$2dqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2fqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$2aqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i32 %b, %a
  ret i32 %0
}

define linkonce i1 @"_Z1$3dqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3eqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i32 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i32 %a, %b
  ret i1 %0
}

define linkonce i32 @"_Z2$3c$3cqi"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = shl i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z2$3e$3eqi"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = lshr i32 %a, %b
  ret i32 %0
}

define linkonce i32 @"_Z1$26qq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$7cqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$5eqq"(i32 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i32 %b, %a
  ret i32 %0
}

define linkonce i32 @"_Z1$7eq"(i32 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i32 %a, -1
  ret i32 %0
}

define linkonce i64 @"_Z1$2brr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$2drr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2frr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2arr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i64 %b, %a
  ret i64 %0
}

define linkonce i1 @"_Z1$3drr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3drr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3crr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp slt i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3drr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sle i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3err"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sgt i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3drr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sge i64 %a, %b
  ret i1 %0
}

define linkonce i64 @"_Z2$3c$3cri"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sext i32 %b to i64
  %1 = shl i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z2$3e$3eri"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sext i32 %b to i64
  %1 = lshr i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z1$26rr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7crr"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$5err"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$2bss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$2dss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2fss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i64 %a, %b
  ret i64 %0
}

define linkonce i64 @"_Z1$2ass"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i64 %b, %a
  ret i64 %0
}

define linkonce i1 @"_Z1$3dss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3css"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3ess"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i64 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i64 %a, %b
  ret i1 %0
}

define linkonce i64 @"_Z2$3c$3csi"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = shl i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z2$3e$3esi"(i64 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i64
  %1 = lshr i64 %a, %0
  ret i64 %1
}

define linkonce i64 @"_Z1$26ss"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7css"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$5ess"(i64 %a, i64 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %b, %a
  ret i64 %0
}

define linkonce i64 @"_Z1$7es"(i64 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i64 %a, -1
  ret i64 %0
}

define linkonce i128 @"_Z1$2btt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$2dtt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i128 %a, %b
  ret i128 %0
}

define linkonce i128 @"_Z1$2ftt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i128 %a, %b
  ret i128 %0
}

define linkonce i128 @"_Z1$2att"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i128 %b, %a
  ret i128 %0
}

define linkonce i1 @"_Z1$3dtt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dtt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3ctt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp slt i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dtt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sle i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3ett"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sgt i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dtt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp sge i128 %a, %b
  ret i1 %0
}

define linkonce i128 @"_Z2$3c$3cti"(i128 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sext i32 %b to i128
  %1 = shl i128 %a, %0
  ret i128 %1
}

define linkonce i128 @"_Z2$3e$3eti"(i128 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = sext i32 %b to i128
  %1 = lshr i128 %a, %0
  ret i128 %1
}

define linkonce i128 @"_Z1$26tt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$7ctt"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$5ett"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$2buu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = add i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$2duu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = sub i128 %a, %b
  ret i128 %0
}

define linkonce i128 @"_Z1$2fuu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = sdiv i128 %a, %b
  ret i128 %0
}

define linkonce i128 @"_Z1$2auu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = mul i128 %b, %a
  ret i128 %0
}

define linkonce i1 @"_Z1$3duu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp eq i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3duu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ne i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cuu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ult i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3duu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ule i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3euu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp ugt i128 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3duu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = icmp uge i128 %a, %b
  ret i1 %0
}

define linkonce i128 @"_Z2$3c$3cui"(i128 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i128
  %1 = shl i128 %a, %0
  ret i128 %1
}

define linkonce i128 @"_Z2$3e$3eui"(i128 %a, i32 %b) nounwind readonly alwaysinline {
entry:
  %0 = zext i32 %b to i128
  %1 = lshr i128 %a, %0
  ret i128 %1
}

define linkonce i128 @"_Z1$26uu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = and i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$7cuu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = or i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$5euu"(i128 %a, i128 %b) nounwind readonly alwaysinline {
entry:
  %0 = xor i128 %b, %a
  ret i128 %0
}

define linkonce i128 @"_Z1$7eu"(i128 %a) nounwind readonly alwaysinline {
entry:
  %0 = xor i128 %a, -1
  ret i128 %0
}

define linkonce float @"_Z1$2bff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fadd float %a, %b
  ret float %0
}

define linkonce float @"_Z1$2dff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fsub float %a, %b
  ret float %0
}

define linkonce float @"_Z1$2fff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fdiv float %a, %b
  ret float %0
}

define linkonce float @"_Z1$2aff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fmul float %a, %b
  ret float %0
}

define linkonce i1 @"_Z1$3dff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp oeq float %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3dff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp one float %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp olt float %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3dff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp ole float %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3eff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp ogt float %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3dff"(float %a, float %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp oge float %a, %b
  ret i1 %0
}

define linkonce double @"_Z1$2bdd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fadd double %a, %b
  ret double %0
}

define linkonce double @"_Z1$2ddd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fsub double %a, %b
  ret double %0
}

define linkonce double @"_Z1$2fdd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fdiv double %a, %b
  ret double %0
}

define linkonce double @"_Z1$2add"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fmul double %a, %b
  ret double %0
}

define linkonce i1 @"_Z1$3ddd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp oeq double %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3ddd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp one double %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3cdd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp olt double %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3ddd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp ole double %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3edd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp ogt double %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3ddd"(double %a, double %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp oge double %a, %b
  ret i1 %0
}

define linkonce x86_fp80 @"_Z1$2baa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fadd x86_fp80 %a, %b
  ret x86_fp80 %0
}

define linkonce x86_fp80 @"_Z1$2daa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fsub x86_fp80 %a, %b
  ret x86_fp80 %0
}

define linkonce x86_fp80 @"_Z1$2faa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fdiv x86_fp80 %a, %b
  ret x86_fp80 %0
}

define linkonce x86_fp80 @"_Z1$2aaa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fmul x86_fp80 %a, %b
  ret x86_fp80 %0
}

define linkonce i1 @"_Z1$3daa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp oeq x86_fp80 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$21$3daa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp one x86_fp80 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3caa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp olt x86_fp80 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3c$3daa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp ole x86_fp80 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z1$3eaa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp ogt x86_fp80 %a, %b
  ret i1 %0
}

define linkonce i1 @"_Z2$3e$3daa"(x86_fp80 %a, x86_fp80 %b) nounwind readonly alwaysinline {
entry:
  %0 = fcmp oge x86_fp80 %a, %b
  ret i1 %0
}

declare %struct__Z3_vp @__retain_struct_0()

declare void @llvm.va_start(%struct__Z3_vp*) nounwind

declare void @llvm.va_end(%struct__Z3_vp*) nounwind

declare %struct__Z5DNode @__retain_struct_1()

declare %"struct__Z7va$2dlist" @__retain_struct_2()

declare %struct__Z8PoolNode @__retain_struct_3()

declare noalias %struct__Z3_vp* @malloc(i64) nounwind

declare void @free(%struct__Z3_vp* nocapture) nounwind

define linkonce_odr i1 @_Z3notb(i1 %a) {
entry:
  %not.a = xor i1 %a, true
  ret i1 %not.a
}

declare %struct__Z3_vp* @memcpy(%struct__Z3_vp*, %struct__Z3_vp* nocapture, i64) nounwind

define linkonce_odr %struct__Z3_vp* @"_Z11pool$2dmallocPZ8PoolNodew"(%struct__Z8PoolNode* %pool-node, i64 %n) {
entry:
  %0 = getelementptr %struct__Z8PoolNode* %pool-node, i64 0, i32 2
  %1 = load %struct__Z8PoolNode** %0, align 8
  %2 = ptrtoint %struct__Z8PoolNode* %1 to i64
  %3 = trunc i64 %2 to i32
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %then, label %done

then:                                             ; preds = %entry
  store %struct__Z8PoolNode* %pool-node, %struct__Z8PoolNode** %0, align 8
  br label %done

done:                                             ; preds = %entry, %then
  %5 = phi %struct__Z8PoolNode* [ %pool-node, %then ], [ %1, %entry ]
  %6 = call %struct__Z3_vp* @malloc(i64 24)
  %7 = bitcast %struct__Z3_vp* %6 to %struct__Z8PoolNode*
  %8 = call %struct__Z3_vp* @malloc(i64 %n)
  %9 = bitcast %struct__Z3_vp* %6 to %struct__Z3_vp**
  store %struct__Z3_vp* null, %struct__Z3_vp** %9, align 8
  %10 = getelementptr %struct__Z3_vp* %6, i64 8
  %11 = bitcast %struct__Z3_vp* %10 to %struct__Z8PoolNode**
  store %struct__Z8PoolNode* null, %struct__Z8PoolNode** %11, align 8
  %12 = getelementptr %struct__Z8PoolNode* %5, i64 0, i32 0
  store %struct__Z3_vp* %8, %struct__Z3_vp** %12, align 8
  %13 = getelementptr %struct__Z8PoolNode* %5, i64 0, i32 1
  store %struct__Z8PoolNode* %7, %struct__Z8PoolNode** %13, align 8
  store %struct__Z8PoolNode* %7, %struct__Z8PoolNode** %0, align 8
  ret %struct__Z3_vp* %8
}

define linkonce_odr void @"_Z9pool$2dfreePZ8PoolNode"(%struct__Z8PoolNode* %pool-node) {
entry:
  %0 = getelementptr %struct__Z8PoolNode* %pool-node, i64 0, i32 0
  %1 = load %struct__Z3_vp** %0, align 8
  %2 = ptrtoint %struct__Z3_vp* %1 to i64
  %3 = trunc i64 %2 to i32
  %4 = icmp eq i32 %3, 0
  %5 = tail call i1 @_Z3notb(i1 %4)
  br i1 %5, label %then, label %done

then:                                             ; preds = %entry
  %6 = load %struct__Z3_vp** %0, align 8
  tail call void @free(%struct__Z3_vp* %6)
  br label %done

done:                                             ; preds = %entry, %then
  %7 = getelementptr %struct__Z8PoolNode* %pool-node, i64 0, i32 1
  %8 = load %struct__Z8PoolNode** %7, align 8
  %9 = ptrtoint %struct__Z8PoolNode* %8 to i64
  %10 = trunc i64 %9 to i32
  %11 = icmp eq i32 %10, 0
  %12 = tail call i1 @_Z3notb(i1 %11)
  br i1 %12, label %then1, label %done3

then1:                                            ; preds = %done
  %13 = load %struct__Z8PoolNode** %7, align 8
  tail call void @"_Z9pool$2dfreePZ8PoolNode"(%struct__Z8PoolNode* %13)
  br label %done3

done3:                                            ; preds = %done, %then1
  %14 = bitcast %struct__Z8PoolNode* %pool-node to %struct__Z3_vp*
  tail call void @free(%struct__Z3_vp* %14)
  ret void
}

define i32 @main() {
entry:
  ret i32 0
}
