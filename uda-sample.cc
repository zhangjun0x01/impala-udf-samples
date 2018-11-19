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
#include "uda-sample.h"
#include <assert.h>
#include <sstream>
#include <iostream>
#include <vector>


using namespace impala_udf;
using namespace std;

template <typename T>
StringVal ToStringVal(FunctionContext* context, const T& val) {
  stringstream ss;
  ss << val;
  string str = ss.str();
  StringVal string_val(context, str.size());
  memcpy(string_val.ptr, str.c_str(), str.size());
  return string_val;
}

template <>
StringVal ToStringVal<DoubleVal>(FunctionContext* context, const DoubleVal& val) {
  if (val.is_null) return StringVal::null();
  return ToStringVal(context, val.val);
}

template <>
StringVal ToStringVal<IntVal>(FunctionContext* context, const IntVal& val) {
  if (val.is_null) return StringVal::null();
  return ToStringVal(context, val.val);
}  


// ---------------------------------------------------------------------------
// This is a sample of implementing a COUNT aggregate function.
// ---------------------------------------------------------------------------
void AllocBuffer(FunctionContext* ctx, StringVal* dst, size_t buf_len) {
  uint8_t* ptr = ctx->Allocate(buf_len);
  if (ptr == NULL && buf_len != 0) {
    *dst = StringVal::null();
  } else {
    *dst = StringVal(ptr, buf_len);
         if (ptr != NULL) memset(ptr, 0, buf_len);
  }
}


struct CountStruct {
  int day;
  int64_t id;
  int step;
  int dst_step;
};


void MyFunnelInit(FunctionContext* context, StringVal* dst) {
    AllocBuffer(context, dst, sizeof(CountStruct));
   CountStruct* state =reinterpret_cast<CountStruct*>(dst->ptr);
   state->day = 0;
   state->id = 0;
   state->step=0; 
   state->dst_step=0; 


}

void MyFunnelUpdate(FunctionContext* context,const IntVal& step, StringVal* val) {

  if (step.is_null) return;
  CountStruct* avg = reinterpret_cast<CountStruct*>(val->ptr);

 //if(day.val == 20180612){
 //      std::cout  <<  "CountUpdate 019 the id is " << id.val << "  the datestamp is  " << datestamp.val << "  the step is " << step.val << "  result value is " << avg->step << endl;
 //}
	//avg->day = day.val;
	//avg->id = id.val;
 	avg->step = step.val;
        if (step.val == (avg->dst_step +1)){
	       ++avg->dst_step;
	}



  //if (step.val == (val->val +1)){
//	++avg->val;
  //}
}

void MyFunnelMerge(FunctionContext* context, const StringVal& src,StringVal* dst) {
  if (src.is_null) return;
  const CountStruct* src_avg = reinterpret_cast<const CountStruct*>(src.ptr);
  CountStruct* dst_avg = reinterpret_cast<CountStruct*>(dst->ptr);


  if(src_avg->dst_step > 1 ){
 	dst_avg->dst_step = src_avg->dst_step;
  }else if(src_avg->step == (dst_avg->dst_step + 1)){
	++dst_avg->dst_step;	
  }
 //else ifsrc_avg->step > dst_avg->step)
 // 	dst_avg->step = src_avg->step;
 // }


 //if(src_avg->day == 20180612){
 //      std::cout  <<  "Countmerge 019 the id is " << src_avg->id  << "  the step is " << src_avg->step  << "  dst step is " << dst_avg->step  <<endl;
 // }


}

StringVal MyFunnelSerialize(FunctionContext* context, const StringVal& val) {
  assert(!val.is_null);
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}


StringVal MyFunnelFinalize(FunctionContext* context, const StringVal& val) {
	  assert(!val.is_null);
  assert(val.len == sizeof(CountStruct));
  CountStruct* avg = reinterpret_cast<CountStruct*>(val.ptr);
  StringVal result = ToStringVal(context, avg->dst_step);
  context->Free(val.ptr);
  return result;
	

}


// This is a sample of implementing a AVG aggregate function.
// ---------------------------------------------------------------------------
struct AvgStruct {
  double sum;
  int64_t count;
};

// Initialize the StringVal intermediate to a zero'd AvgStruct
void AvgInit(FunctionContext* context, StringVal* val) {
  val->is_null = false;
  val->len = sizeof(AvgStruct);
  val->ptr = context->Allocate(val->len);
  memset(val->ptr, 0, val->len);
}

void AvgUpdate(FunctionContext* context, const DoubleVal& input, StringVal* val) {
  if (input.is_null) return;
  assert(!val->is_null);
  assert(val->len == sizeof(AvgStruct));
  AvgStruct* avg = reinterpret_cast<AvgStruct*>(val->ptr);
  avg->sum += input.val;
  ++avg->count;
}

void AvgMerge(FunctionContext* context, const StringVal& src, StringVal* dst) {
  if (src.is_null) return;
  const AvgStruct* src_avg = reinterpret_cast<const AvgStruct*>(src.ptr);
  AvgStruct* dst_avg = reinterpret_cast<AvgStruct*>(dst->ptr);
  dst_avg->sum += src_avg->sum;
  dst_avg->count += src_avg->count;
}

// A serialize function is necesary to free the intermediate state allocation. We use the
// StringVal constructor to allocate memory owned by Impala, copy the intermediate state,
// and free the original allocation. Note that memory allocated by the StringVal ctor is
// not necessarily persisted across UDA function calls, which is why we don't use it in
// AvgInit().
StringVal AvgSerialize(FunctionContext* context, const StringVal& val) {
  assert(!val.is_null);
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}

StringVal AvgFinalize(FunctionContext* context, const StringVal& val) {
  assert(!val.is_null);
  assert(val.len == sizeof(AvgStruct));
  AvgStruct* avg = reinterpret_cast<AvgStruct*>(val.ptr);
  StringVal result;
  if (avg->count == 0) {
    result = StringVal::null();
  } else {
    // Copies the result to memory owned by Impala
    result = ToStringVal(context, avg->sum / avg->count);
  }
  context->Free(val.ptr);
  return result;
}




void PageInit(FunctionContext* context, BigIntVal* val) {
  val->is_null = false;
  val->val = 0;
}

void PageUpdate(FunctionContext* context, const StringVal& input, BigIntVal* val) {
  if (input.is_null) return;
  AvgStruct* dst_avg = reinterpret_cast<AvgStruct*>(input.ptr);
  *val = dst_avg->count;
}

void PageMerge(FunctionContext* context, const BigIntVal& src, BigIntVal* dst) {
  dst->val += src.val;
}
              
BigIntVal PageFinalize(FunctionContext* context, const BigIntVal& val) {
  return val;
}  



// ---------------------------------------------------------------------------
// This is a sample of implementing the STRING_CONCAT aggregate function.
// Example: select string_concat(string_col, ",") from table
// ---------------------------------------------------------------------------
// Delimiter to use if the separator is NULL.
static const StringVal DEFAULT_STRING_CONCAT_DELIM((uint8_t*)", ", 2);

void StringConcatInit(FunctionContext* context, StringVal* val) {
  val->is_null = true;
}

void StringConcatUpdate(FunctionContext* context, const StringVal& str,
    const StringVal& separator, StringVal* result) {
  if (str.is_null) return;
  if (result->is_null) {
    // This is the first string, simply set the result to be the value.
    uint8_t* copy = context->Allocate(str.len);
    memcpy(copy, str.ptr, str.len);
    *result = StringVal(copy, str.len);
    return;
  }

  const StringVal* sep_ptr = separator.is_null ? &DEFAULT_STRING_CONCAT_DELIM :
      &separator;

  // We need to grow the result buffer and then append the new string and
  // separator.
  int new_size = result->len + sep_ptr->len + str.len;
  result->ptr = context->Reallocate(result->ptr, new_size);
  memcpy(result->ptr + result->len, sep_ptr->ptr, sep_ptr->len);
  result->len += sep_ptr->len;
  memcpy(result->ptr + result->len, str.ptr, str.len);
  result->len += str.len;
}

void StringConcatMerge(FunctionContext* context, const StringVal& src, StringVal* dst) {
  if (src.is_null) return;
  StringConcatUpdate(context, src, ",", dst);
}

// A serialize function is necesary to free the intermediate state allocation. We use the
// StringVal constructor to allocate memory owned by Impala, copy the intermediate
// StringVal, and free the intermediate's memory. Note that memory allocated by the
// StringVal ctor is not necessarily persisted across UDA function calls, which is why we
// don't use it in StringConcatUpdate().
StringVal StringConcatSerialize(FunctionContext* context, const StringVal& val) {
  if (val.is_null) return val;
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}

// Same as StringConcatSerialize().
StringVal StringConcatFinalize(FunctionContext* context, const StringVal& val) {
  if (val.is_null) return val;
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}







struct PageStruct {
  int index;
  int size;
  vector<StringVal> pages;
};

static char* LocateSubstring(char* haystack, int hay_len, const char* needle, int needle_len) {
  for (int i = 0; i < hay_len - needle_len + 1; ++i) {
    char* possible_needle = haystack + i;
    if (strncmp(possible_needle, needle, needle_len) == 0) return possible_needle;
  }
  return NULL;
}



void SplitPart(const StringVal& str, const StringVal& delim,vector<StringVal>& container) {
  if (str.is_null || delim.is_null) return;
  if (delim.len == 0) return;
  char* str_start = reinterpret_cast<char*>(str.ptr);
  char* str_part = str_start;
  char* delimiter = reinterpret_cast<char*>(delim.ptr);
  for (int cur_pos = 1; ; ++cur_pos) {
    int remaining_len = str.len - (str_part - str_start);
    char* delim_ref = LocateSubstring(str_part, remaining_len, delimiter, delim.len);
    if (delim_ref == NULL) {
        container.push_back(StringVal(reinterpret_cast<uint8_t*>(str_part), remaining_len));
        break;
    }
    container.push_back(StringVal(reinterpret_cast<uint8_t*>(str_part),delim_ref - str_part));
    str_part = delim_ref + delim.len;
  }
}


void FindMaxPageInit(FunctionContext* context, StringVal* val){
  val->is_null = true;
}


void FindMaxPageUpdate(FunctionContext* context, const StringVal& col,const StringVal& pages, const StringVal& delim,StringVal* result){
  if (col.is_null) return;

  if (result->is_null) {
	result->is_null = false;
  	result->len = sizeof(PageStruct);
  	result->ptr = context->Allocate(result->len);
  	memset(result->ptr, 0, result->len); 
        PageStruct* avg = reinterpret_cast<PageStruct*>(result->ptr);
        SplitPart(pages,delim,avg->pages); 
	avg->index  = 0 ;
        avg->size = avg->pages.size();

         if(avg->pages[0] == col){
               ++avg->index;
         }else {
	 }
  }else{
   	PageStruct* avg = reinterpret_cast<PageStruct*>(result->ptr);
        if(avg->index >= avg->size) return;

        if(avg->pages[avg->index] == col){
              ++avg->index;
         }
 	 
  }
 

}


void FindMaxPageMerge(FunctionContext* context, const StringVal& src, StringVal* dst) {
}
StringVal FindMaxPageFinalize(FunctionContext* context, const StringVal& val){
        if (val.is_null) return val;
        context->Free(val.ptr);
}




// ---------------------------------------------------------------------------
// peng.liu3 2018-04-02: funnel compute method
// ---------------------------------------------------------------------------
void FunnelInit(FunctionContext* context, IntVal* val) {
  val->is_null = false;
  val->val = 0;
}

void FunnelUpdate(FunctionContext* context,const IntVal& step, IntVal* val) {
//void FunnelUpdate(FunctionContext* context,const IntVal& day,const BigIntVal& datestamp,const BigIntVal& id,const IntVal& step, IntVal* val) {
 // if(day.val == 20180612){
  //	std::cout  <<  "FunnelUpdate the id is " << id.val << "  the datestamp is  " << datestamp.val << "  the step is " << step.val << "  result value is " << val->val << endl;
  //}
  if (step.is_null) return;
  if (step.val == val->val +1){
	val->val = val->val + 1;
    	//++val->val;
	//if(day.val == 20180612){
	//	  std::cout << "FunnelUpdate  result , the id is " << id.val << "  the datestamp is  " << datestamp.val << "  the step is " << step.val << "  result value is " << val->val << endl;
	//}
	
  }
}

void FunnelMerge(FunctionContext* context, const IntVal& src, IntVal* dst) {
  	//cout << "FunnelMerge src values "  << src.val << "   dst value is " << dst->val << endl; 
  if (src.val >= dst->val ){
    dst->val = src.val;
  }
  //else{
  //  dst->val;
  //}

}

IntVal FunnelFinalize(FunctionContext* context, const IntVal& val) {
  return val;
}

struct NameSpacePair{
StringVal name_space;
int num;
};

struct PathStruct {
  int step;
  int64_t timestamp;
  bool hasCalc;// session有效期内已经计算了一个步骤
  std::vector<NameSpacePair> path_map;
};
void FindPathInit(FunctionContext* context,StringVal* val){
  val->is_null = false;
  val->len = sizeof(PathStruct);
  val->ptr = context->Allocate(val->len);
  memset(val->ptr, 0, val->len);
}

void addMap(std::vector<NameSpacePair>& path_map,const StringVal& name_space){
	bool exist = false;
	NameSpacePair pair;
        pair.name_space = name_space;
	for(int i= 0; i< path_map.size();i++){
		if(path_map[i].name_space == name_space){
			exist = true;
			pair.num = path_map[i].num + 1;

			path_map[i] = pair;
			break;
		}	
	}
	
	if(!exist){
		pair.num = 1;
		path_map.push_back(pair);
	}

}


void FindPathUpdate(FunctionContext* context,const IntVal& step,const StringVal& name_space,const BigIntVal& timestamp,const IntVal& interval,const IntVal& maxStep,StringVal* res){
	PathStruct* ps = reinterpret_cast<PathStruct*>(res->ptr);
	cout << "FindPathUpdate , the step is " << step.val << endl;


	// the first row
 	if(ps->timestamp == 0){
		ps->timestamp  = timestamp.val;
		ps->step = step.val;
	}else{
	    //beyond session 
	    if((timestamp.val - ps->timestamp) > interval.val){
		//reset session begin time 
			ps->timestamp = timestamp.val;
	    }else{
	       if(ps->step == 0){
                        ps->step = step.val;
                }else{
                        if(ps->step == maxStep.val && !ps->hasCalc){
				addMap(ps->path_map,name_space);
			   	ps->hasCalc = true;
                        }
                }	
			
	    }	
	}

}


StringVal FindPathSerialize(FunctionContext* context, const StringVal& val) {
  if (val.is_null) return val;
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}

void FindPathMerge(FunctionContext* context, const StringVal& src, StringVal* dst){
  if (src.is_null) return;
  const PathStruct* src_avg = reinterpret_cast<const PathStruct*>(src.ptr);
  PathStruct* dst_avg = reinterpret_cast<PathStruct*>(dst->ptr);
  dst_avg->path_map = src_avg->path_map;


}
StringVal FindPathFinalize(FunctionContext* context, const StringVal& val){
 	PathStruct* dst_avg = reinterpret_cast<PathStruct*>(val.ptr);
	int size  = dst_avg->path_map.size();
	cout << "FindPathFinalize ,the size is " << size <<  endl;
	for(int i = 0; i < size;i++){
		cout << "the num is " << dst_avg->path_map[i].num << endl;
	}
	context->Free(val.ptr);
	return ToStringVal(context,100);
}







//find next step 

struct FindNextStepStruct{
	int step;
	int name_space_key;
	bool finished ;
	int index;
	vector<int> stepVec;
};


void FindNextStepInit(FunctionContext* ctx,StringVal* dst ){
  AllocBuffer(ctx, dst, sizeof(FindNextStepStruct));
  FindNextStepStruct* state =reinterpret_cast<FindNextStepStruct*>(dst->ptr);
  state->step = 0;
  state->name_space_key = 0;
  state->finished = false;
  state->index=0;
}

void parseInt(const StringVal& input, vector<int>& v){

  uint8_t *ptr;
  int index = 0;  
  int num = -1;
  for (ptr = input.ptr, index = 0; index < input.len; index++, ptr++) {
	uint8_t c = tolower(*ptr);
	if(num == -1 && c != ','){
		num = c - 48; 
	}else if(c != ','){
	 	num = num*10 + (c - 48);			
	}

	 if(c == ',' || index == input.len - 1 ){
		v.push_back(num);
		num = -1;
	}
  }



}


void FindNextStepUpdate(FunctionContext* context,const IntVal& root,const IntVal& key,const StringVal& steps,StringVal* res){
	assert(!root.is_null);
	

	FindNextStepStruct* fs = reinterpret_cast<FindNextStepStruct*>(res->ptr);
	if(fs->finished){return;}
	if(fs->stepVec.size() == 0) {
		parseInt(steps,fs->stepVec);
	}
	
	
	// 直到找到第一条符合第一步骤的数据
	if(root.val != 0 && fs->index == 0){
		return;
	}
	


	if(fs->index !=0 && fs->index == fs->stepVec.size()){
		fs->finished = true;
		fs->name_space_key = key.val;
		return;
	}

	if(fs->stepVec.size() == 1 && fs->stepVec[fs->index] == root.val){
		++fs->index;
		if(fs->stepVec[0] == root.val){
                           fs->name_space_key = -1;
                }
	}else if(fs->stepVec.size() > 1 && fs->index != -1){
		if((fs->index == 0 && fs->stepVec[0] == root.val ) || (fs->index > 0 && fs->stepVec[fs->index] == key.val )  ){
			++fs->index;
		}else if(fs->index > 0 && fs->stepVec[fs->index] != key.val){
			 fs->index = -1;
		}
		
		if(fs->index == fs->stepVec.size() && fs->stepVec[fs->stepVec.size()-1] == key.val){
                	fs->name_space_key = -1;
                }
			
	}
	 
}

StringVal FindNextStepSerialize(FunctionContext* context, const StringVal& val) {
  assert(!val.is_null);
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}


void FindNextStepMerge(FunctionContext* context, const StringVal& src, StringVal* dst){ 
  const FindNextStepStruct* src_avg = reinterpret_cast<const FindNextStepStruct*>(src.ptr);
  FindNextStepStruct* dst_avg = reinterpret_cast<FindNextStepStruct*>(dst->ptr);
  dst_avg->name_space_key = src_avg->name_space_key;

}

StringVal FindNextStepFinalize(FunctionContext* context, const StringVal& val){

    FindNextStepStruct* src_avg = reinterpret_cast<FindNextStepStruct*>(val.ptr);    

    StringVal result = ToStringVal(context,src_avg->name_space_key);
    
    context->Free(val.ptr);
    return result;
}




struct FindPreStepStruct{
        int step;
        int name_space_key;
        bool finished ;
        int index;
        vector<int> stepVec;
        vector<int> tmpVec;
};
void FindPreStepInit(FunctionContext* ctx,StringVal* dst){

  AllocBuffer(ctx, dst, sizeof(FindPreStepStruct));
  FindPreStepStruct* state =reinterpret_cast<FindPreStepStruct*>(dst->ptr);
  state->step = 0;
  state->name_space_key = 0;
  state->finished = false;
  state->index=0;

}

void addData(vector<int>& tmpVec,int root,int key,int size){
	if(root == 0){
		tmpVec.push_back(root);		
	}else{
		tmpVec.push_back(key);
	}

	if(tmpVec.size()>size){
		tmpVec.erase(tmpVec.begin());
	}
		

}


bool compare(vector<int>& tmpVec,vector<int>& stepsVec){
	bool finish = true;
	for(int i=0,j=tmpVec.size();i<stepsVec.size(),j>0;i++,j--){
		if(tmpVec[j] != stepsVec[i]){
			finish = false;	
		}
	}
	return finish;
}


void FindPreStepUpdate(FunctionContext* context,const IntVal& root,const IntVal& key,const StringVal& steps,StringVal* res){
	assert(!root.is_null);
        FindPreStepStruct* fs = reinterpret_cast<FindPreStepStruct*>(res->ptr);
        if(fs->finished){return;}
        if(fs->stepVec.size() == 0) {
                parseInt(steps,fs->stepVec);
        }

	addData(fs->tmpVec,root.val,key.val,fs->stepVec.size());

	if(root.val == 0){
		if(compare(fs->tmpVec,fs->stepVec)){
			fs->finished = true;
			fs->name_space_key = key.val;
		}
	}



}


StringVal FindPreStepSerialize(FunctionContext* context, const StringVal& val) {
  assert(!val.is_null);
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}

void FindPreStepMerge(FunctionContext* context, const StringVal& src, StringVal* dst){
  const FindPreStepStruct* src_avg = reinterpret_cast<const FindPreStepStruct*>(src.ptr);
  FindPreStepStruct* dst_avg = reinterpret_cast<FindPreStepStruct*>(dst->ptr);
  dst_avg->name_space_key = src_avg->name_space_key;
}


StringVal FindPreStepFinalize(FunctionContext* context, const StringVal& val){
    FindPreStepStruct* src_avg = reinterpret_cast<FindPreStepStruct*>(val.ptr);
    StringVal result = ToStringVal(context,src_avg->name_space_key);
    context->Free(val.ptr);
    return result;
}






////////////////////////////
//
// funnel with window
//
//
/////////////////////////

struct FunnelWindowStruct {
  int step;
  int64_t datestamp;
};

void FunnelWindowInit(FunctionContext* context, StringVal* val){
  val->is_null = false;
  val->len = sizeof(FunnelWindowStruct);
  val->ptr = context->Allocate(val->len);
  memset(val->ptr, 0, val->len);
  //AllocBuffer(context, val, sizeof(FunnelWindowStruct));
  //FunnelWindowStruct* state =reinterpret_cast<FunnelWindowStruct*>(val->ptr);
  //state->step = 0;
}

//*******************************
//
//查询窗口期是${day}天，窗口期的第一天是${beginDay}的漏斗转换。
//漏斗的第一步必须发生在窗口期的第一天,其他步骤没有限制,在窗口期内即可
//
//
//
void FunnelWindowUpdate(FunctionContext* context,const IntVal& beginDay ,const IntVal& day, const BigIntVal& datestamp,const BigIntVal& interval,const IntVal& step, StringVal* val){
	FunnelWindowStruct* fws = reinterpret_cast<FunnelWindowStruct*>(val->ptr);
	//cout << "update ,the step is " << step.val << " fws->step is " << fws->step << " datestamp is " << datestamp.val <<  " fws->datestamp is  " << fws->datestamp <<  endl;
	//漏斗的第一步必须发生在窗口期的第一天，如果还没有走到第二步的时候走了多个第一步，取最后一个
    	if(beginDay.val == day.val && step.val == 1 && fws->step <=1 ){
		fws->step = 1;
		fws->datestamp = datestamp.val;
		//cout << "add first step " << endl;
	}	

	//if(step.val > 1){
	//	cout << "step is " << step.val << " datestamp.val - fws->datestamp " << (datestamp.val - fws->datestamp)<<" interval.val " << interval.val << " fws->step " << fws->step << endl;
	//}
        if(step.val > 1 && (datestamp.val - fws->datestamp ) <= interval.val && (fws->step+1) == step.val) {
		fws->step = fws->step + 1;	
		//std::cout << "update step is " << fws->step << endl;
	}	

}

void FunnelWindowMerge(FunctionContext* context, const StringVal& src, StringVal* dst){
  const FunnelWindowStruct* src_avg = reinterpret_cast<const FunnelWindowStruct*>(src.ptr);
  FunnelWindowStruct* dst_avg = reinterpret_cast<FunnelWindowStruct*>(dst->ptr);
  if(dst_avg->step > src_avg->step ){
	dst_avg->step = src_avg->step;
  }

}

StringVal FunnelWindowSerialize(FunctionContext* context, const StringVal& val){
  assert(!val.is_null);
  StringVal result(context, val.len);
  memcpy(result.ptr, val.ptr, val.len);
  context->Free(val.ptr);
  return result;
}

StringVal FunnelWindowFinalize(FunctionContext* context, const StringVal& val){
  assert(!val.is_null);
  assert(val.len == sizeof(FunnelWindowStruct));
  FunnelWindowStruct* avg = reinterpret_cast<FunnelWindowStruct*>(val.ptr);
  //cout << "FunnelWindowFinalize final step " << avg->step << endl;
  StringVal result = ToStringVal(context,avg->step);
  context->Free(val.ptr);
  return result;

}

