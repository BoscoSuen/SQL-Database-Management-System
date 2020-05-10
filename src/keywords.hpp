//
//  keywords.hpp
//  Database
//
//  Created by rick gessner on 3/21/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef keywords_h
#define keywords_h

#include <string>
#include <map>
#include "Errors.hpp"

namespace ECE141 {
  
  //This defines the keywords we need to handle across our multiple languages...
  enum class Keywords {
    add_kw=1, all_kw, alter_kw, and_kw, as_kw, avg_kw, auto_increment_kw,
    between_kw, boolean_kw, by_kw,
    char_kw, column_kw, count_kw, create_kw, cross_kw,
    database_kw, databases_kw, datetime_kw, decimal_kw, delete_kw,
    describe_kw, distinct_kw, double_kw, drop_kw, explain_kw,
    float_kw, foreign_kw, from_kw, full_kw, group_kw, help_kw,
    in_kw, inner_kw, insert_kw, integer_kw, into_kw, join_kw, key_kw, left_kw,
    max_kw, min_kw, modify_kw, not_kw,  null_kw, on_kw, or_kw, order_kw, outer_kw,
    primary_kw, quit_kw, references_kw, right_kw,
    select_kw, self_kw, set_kw, show_kw, sum_kw,
    table_kw, tables_kw, unknown_kw, unique_kw, update_kw, use_kw,
    values_kw, varchar_kw, version_kw, where_kw,
  };
  
  //This enum defines operators that will be used in SQL commands...
  enum class Operators {
    equal_op=1, notequal_op, lt_op, lte_op, gt_op, gte_op, between_op,
    or_op, nor_op, and_op, not_op, dot_op,
    add_op, subtract_op, multiply_op, divide_op, power_op, mod_op,
    unknown_op
  };
  
}

#endif /* keywords_h */
