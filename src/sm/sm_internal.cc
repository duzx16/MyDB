#include "sm_internal.h"

ExprNode::ExprNode(int i) {
	is_null = false;
    value_i = i;
    nodeType = NodeType::CONST_NODE;
    attrType = AttrType::INT;
}

ExprNode::ExprNode(float f) {
	is_null = false;
    value_f = f;
    nodeType = NodeType::CONST_NODE;
    attrType = AttrType::FLOAT;
}

ExprNode::ExprNode(const char *s) {
	is_null = false;
    value_s = std::string(s, 1, strlen(s) - 2);;
    nodeType = NodeType::CONST_NODE;
    attrType = AttrType::STRING;
}

ExprNode::ExprNode(int left, ArithOp arithOp, int right) {
	is_null = false;
    this->left = left;
    this->right = right;
    this->arithOp = op;
    this->nodeType = NodeType::ARITH_NODE;
}

ExprNode::ExprNode(int left, CompOp arithOp, int right) {
	is_null = false;
    this->left = left;
    this->right = right;
    this->compOp = op;
    this->nodeType = NodeType::COMP_NODE;
}

ExprNode::ExprNode(AttributeNode attribute) {
	is_null = false;
    this->attribute = attributeNode;
    this->nodeType = NodeType::ATTR_NODE;
}

int getExprNode(Expr *expr, int &curSize, ExprNode *exprs) {
	int d = curSize++;
	ExprNode exprNode = &(exprs[d]);
	if (expr->nodeType == NodeType::CONST_NODE) {
		if (expr->attrType == AttrType::INT) {
			ExprNode _(expr->value_i);
			exprNode = _;
		}
		if (expr->attrType == AttrType::FLOAT) {
			ExprNode _(expr->value_f);
			exprNode = _;
		}
		if (expr->attrType == AttrType::STRING) {
			ExprNode _(expr->value_s);
			exprNode = _;
		}
		if (expr->attrType == AttrType::NO_ATTR) {
			ExprNode _();
			exprNode = _;
		}
	}
	else if (expr->nodeType == NodeType::ARITH_NODE) {
		int left = -1, right = -1;
		if (expr->left != nullptr)
			left = getExprNode(expr->left, curSize, exprs);
		if (expr->right != nullptr)
			right = getExprNode(expr->right, curSize, exprs);
		ExprNode _(left, expr->arithOp, right);
		exprNode = _;
	}
	else if (expr->nodeType == NodeType::COMP_NODE) {
		int left = -1, right = -1;
		if (expr->left != nullptr)
			left = getExprNode(expr->left, curSize, exprs);
		if (expr->right != nullptr)
			right = getExprNode(expr->right, curSize, exprs);
		ExprNode _(left, expr->compOp, right);
		exprNode = _;
	}
	else if (expr->nodeType == NodeType::ATTR_NODE) {
		ExprNode _(expr->attribute);
		exprNode = _;
	}
	return d;
}
ExprTree getExprTreeFromExpr(Expr *expr) {
	ExprTree exprTree;
	getExprNode(expr, exprTree.count, exprTree.exprs);
	return exprTree;
}
Expr* getExprFromExprTree(ExprTree &exprTree) {
	return getExprFromExprNode(0, exprTree.exprs);
}

Expr* getExprFromExprNode(int id, ExprNode *exprs) {
	ExprNode *exprNode = &(exprs[id]);
	if (exprNode->nodeType == NodeType::CONST_NODE) {
		if (exprNode->attrType == AttrType::INT) {
			return new Expr(exprNode->value_i);
		}
		if (exprNode->attrType == AttrType::FLOAT) {
			return new Expr(exprNode->value_f);
		}
		if (exprNode->attrType == AttrType::STRING) {
			return new Expr((exprNode->value_s).c_str());
		}
		if (exprNode->attrType == AttrType::NO_ATTR) {
			return new Expr();
		}
	}
	else if (exprNode->nodeType == NodeType::ARITH_NODE) {
		Expr *left, *right;
		left = (exprNode->left == -1) ? nullptr : getExprFromExprNode(exprNode->left, exprs);
		right = (exprNode->right == -1) ? nullptr : getExprFromExprNode(exprNode->right, exprs);
		return new Expr(left, exprNode->arithOp, right);
	}
	else if (exprNode->nodeType == NodeType::COMP_NODE) {
		Expr *left, *right;
		left = (exprNode->left == -1) ? nullptr : getExprFromExprNode(exprNode->left, exprs);
		right = (exprNode->right == -1) ? nullptr : getExprFromExprNode(exprNode->right, exprs);
		return new Expr(left, exprNode->compOp, right);
	}
	else if (exprNode->nodeType == NodeType::ATTR_NODE) {
		return new Expr(exprNode->attribute);
	}
}