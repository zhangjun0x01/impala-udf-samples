// Copyright 2012 Cloudera Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef SAMPLES_UDA_H
#define SAMPLES_UDA_H

#include <impala_udf/udf.h>

using namespace impala_udf;

// Note: As of Impala 1.2, UDAs must have the same intermediate and result types (see the
// udf.h header for the full Impala UDA specification, which can be found at
// https://github.com/cloudera/impala/blob/master/be/src/udf/udf.h). Some UDAs naturally
// conform to this limitation, such as Count and StringConcat. However, other UDAs return
// a numeric value but use a custom intermediate struct type that must be stored in a
// StringVal, such as Variance.
//
// As a workaround for now, these UDAs that require an intermediate buffer use StringVal
// for the intermediate and result type. In the UDAs' finalize functions, the numeric
// result is serialized to an ASCII string (see the ToStringVal() utility function
// provided with these samples). The returned StringVal is then cast back to the correct
// numeric type (see the Usage examples below).
//
// This restriction will be lifted in Impala 2.0.


// This is an example of the COUNT aggregate function.
//
// Usage: > create aggregate function my_count(int) returns bigint
//          location '/user/cloudera/libudasample.so' update_fn='CountUpdate';
//        > select my_count(col) from tbl;
void MyFunnelInit(FunctionContext* context, StringVal* val);
void MyFunnelUpdate(FunctionContext* context, const IntVal& step, StringVal* val);
void MyFunnelMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal MyFunnelSerialize(FunctionContext* context, const StringVal& val);
StringVal MyFunnelFinalize(FunctionContext* context, const StringVal& val);


void PageInit(FunctionContext* context, BigIntVal* val);
void PageUpdate(FunctionContext* context, const StringVal& input, BigIntVal* val);
void PageMerge(FunctionContext* context, const BigIntVal& src, BigIntVal* dst);
BigIntVal PageFinalize(FunctionContext* context, const BigIntVal& val);


// This is an example of the AVG(double) aggregate function. This function needs to
// maintain two pieces of state, the current sum and the count. We do this using
// the StringVal intermediate type. When this UDA is registered, it would specify
// 16 bytes (8 byte sum + 8 byte count) as the size for this buffer.
//
// Usage: > create aggregate function my_avg(double) returns string 
//          location '/user/cloudera/libudasample.so' update_fn='AvgUpdate';
//        > select cast(my_avg(col) as double) from tbl;
void AvgInit(FunctionContext* context, StringVal* val);
void AvgUpdate(FunctionContext* context, const DoubleVal& input, StringVal* val);
void AvgMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal AvgSerialize(FunctionContext* context, const StringVal& val);
StringVal AvgFinalize(FunctionContext* context, const StringVal& val);

// This is a sample of implementing the STRING_CONCAT aggregate function.
//
// Usage: > create aggregate function string_concat(string, string) returns string
//          location '/user/cloudera/libudasample.so' update_fn='StringConcatUpdate';
//        > select string_concat(string_col, ",") from table;
void StringConcatInit(FunctionContext* context, StringVal* val);
void StringConcatUpdate(FunctionContext* context, const StringVal& arg1,
    const StringVal& arg2, StringVal* val);
void StringConcatMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal StringConcatSerialize(FunctionContext* context, const StringVal& val);
StringVal StringConcatFinalize(FunctionContext* context, const StringVal& val);

// This is a example of the variance aggregate function.
//
// Usage: > create aggregate function var(double) returns string
//          location '/user/cloudera/libudasample.so' update_fn='VarianceUpdate';
//        > select cast(var(col) as double) from tbl;
void VarianceInit(FunctionContext* context, StringVal* val);
void VarianceUpdate(FunctionContext* context, const DoubleVal& input, StringVal* val);
void VarianceMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal VarianceSerialize(FunctionContext* context, const StringVal& val);
StringVal VarianceFinalize(FunctionContext* context, const StringVal& val);

// An implementation of the Knuth online variance algorithm, which is also single pass and
// more numerically stable.
//
// Usage: > create aggregate function knuth_var(double) returns string
//          location '/user/cloudera/libudasample.so' update_fn='KnuthVarianceUpdate';
//        > select cast(knuth_var(col) as double) from tbl;
void KnuthVarianceInit(FunctionContext* context, StringVal* val);
void KnuthVarianceUpdate(FunctionContext* context, const DoubleVal& input, StringVal* val);
void KnuthVarianceMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal KnuthVarianceSerialize(FunctionContext* context, const StringVal& val);
StringVal KnuthVarianceFinalize(FunctionContext* context, const StringVal& val);



void FindMaxPageInit(FunctionContext* context, StringVal* val);
void FindMaxPageUpdate(FunctionContext* context, const StringVal& col,const StringVal& pages, const StringVal& delim,StringVal* result);
void FindMaxPageMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal FindMaxPageSerialize(FunctionContext* context, const StringVal& val);
StringVal FindMaxPageFinalize(FunctionContext* context, const StringVal& val);


// The different steps of the UDA are composable. In this case, we'the UDA will use the
// other steps from the Knuth variance computation.
//
// Usage: > create aggregate function stddev(double) returns string
//          location '/user/cloudera/libudasample.so' update_fn='KnuthVarianceUpdate'
//          finalize_fn="StdDevFinalize";
//        > select cast(stddev(col) as double) from tbl;
StringVal StdDevFinalize(FunctionContext* context, const StringVal& val);

// Utility function for serialization to StringVal
template <typename T>
StringVal ToStringVal(FunctionContext* context, const T& val);


// ---------------------------------------------------------------------------
// peng.liu3 2018-04-02: funnel compute method
// ---------------------------------------------------------------------------
void FunnelInit(FunctionContext* context, IntVal* val);
void FunnelUpdate(FunctionContext* context,const IntVal& step, IntVal* val);
//void FunnelUpdate(FunctionContext* context,const IntVal& day,const BigIntVal& datestamp,const BigIntVal& id,const IntVal& step, IntVal* val);
void FunnelMerge(FunctionContext* context, const IntVal& src, IntVal* dst);
IntVal FunnelFinalize(FunctionContext* context, const IntVal& val);

//user path with use session ,not finish 
void FindPathInit(FunctionContext* context,StringVal* val);
void FindPathUpdate(FunctionContext* context,const IntVal& step,const StringVal& name_space,const BigIntVal& timestamp,const IntVal& interval,const IntVal& maxStep,StringVal* res);
void FindPathMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal FindPathFinalize(FunctionContext* context, const StringVal& val);

//user path without user session
void FindNextStepInit(FunctionContext* context,StringVal* val);
void FindNextStepUpdate(FunctionContext* context,const IntVal& root,const IntVal& key,const StringVal& steps,StringVal* res);
void FindNextStepMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal FindNextStepFinalize(FunctionContext* context, const StringVal& val);



void FindPreStepInit(FunctionContext* context,StringVal* val);
void FindPreStepUpdate(FunctionContext* context,const IntVal& root,const IntVal& key,const StringVal& steps,StringVal* res);
void FindPreStepMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal FindPreStepFinalize(FunctionContext* context, const StringVal& val);




#endif
