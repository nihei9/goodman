#ifndef good_PRODUCTION_RULES_H
#define good_PRODUCTION_RULES_H

#include <collections.h>
#include <stdio.h>

typedef struct good_ProductionRule {
	unsigned int id;
	syms_SymbolID lhs;
	syms_SymbolID *rhs;
	size_t rhs_len;
	int is_empty;
} good_ProductionRule;

typedef struct good_ProductionRules good_ProductionRules;

good_ProductionRules *good_new_prules(void);
void good_delete_prules(good_ProductionRules *prules);
int good_append_prule(good_ProductionRules *prules, syms_SymbolID lhs, const syms_SymbolID *rhs, size_t rhs_len);
size_t good_get_prules_len(const good_ProductionRules *prules);
const good_ProductionRule *good_get_prule(const good_ProductionRules *prules, unsigned int id);

typedef struct good_ProductionRuleFilter {
	arr_Filter prs_filter;
} good_ProductionRuleFilter;

good_ProductionRuleFilter *good_set_prule_filter_by_id(good_ProductionRuleFilter *filter, unsigned int id);
good_ProductionRuleFilter *good_set_prule_filter_by_lhs(good_ProductionRuleFilter *filter, syms_SymbolID lhs);
good_ProductionRuleFilter *good_set_prule_filter_match_all(good_ProductionRuleFilter *filter);
const good_ProductionRule *good_next_prule(good_ProductionRuleFilter *filter, const good_ProductionRules *prules);

#endif