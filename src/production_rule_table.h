#ifndef grm_PRODUCTION_RULE_TABLE_H
#define grm_PRODUCTION_RULE_TABLE_H

#include "symbol_table.h"
#include <collections.h>
#include <stdio.h>

typedef struct grm_ProductionRule grm_ProductionRule;

unsigned int grm_get_pr_id(const grm_ProductionRule *pr);
good_SymbolID grm_get_pr_lhs(const grm_ProductionRule *pr);
const good_SymbolID *grm_get_pr_rhs(const grm_ProductionRule *pr);
size_t grm_get_pr_rhs_len(const grm_ProductionRule *pr);

typedef struct grm_ProductionRuleTable grm_ProductionRuleTable;

grm_ProductionRuleTable *grm_new_prtbl(void);
void grm_delete_prtbl(grm_ProductionRuleTable *prtbl);
int grm_append_to_prtbl(grm_ProductionRuleTable *prtbl, good_SymbolID lhs, const good_SymbolID *rhs, size_t rhs_len);
size_t grm_get_prtbl_len(const grm_ProductionRuleTable *prtbl);

typedef struct grm_ProductionRuleFilter {
	arr_Filter prs_filter;
} grm_ProductionRuleFilter;

grm_ProductionRuleFilter *grm_set_pr_filter_by_id(grm_ProductionRuleFilter *filter, unsigned int id);
grm_ProductionRuleFilter *grm_set_pr_filter_by_lhs(grm_ProductionRuleFilter *filter, good_SymbolID lhs);
grm_ProductionRuleFilter *grm_set_pr_filter_match_all(grm_ProductionRuleFilter *filter);
const grm_ProductionRule *grm_next_pr(grm_ProductionRuleFilter *filter, const grm_ProductionRuleTable *prtbl);

#endif