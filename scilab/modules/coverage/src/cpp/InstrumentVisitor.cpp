/*
 *  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 *  Copyright (C) 2015 - Scilab Enterprises - Calixte DENIZET
 *
 *  This file must be used under the terms of the CeCILL.
 *  This source file is licensed as described in the file COPYING, which
 *  you should have received as part of this distribution.  The terms
 *  are also available at
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

#include "InstrumentVisitor.hxx"
#include "CoverModule.hxx"

namespace coverage
{

void InstrumentVisitor::visit(ast::SeqExp & e)
{
    if (e.getParent() && !e.getParent()->isFunctionDec())
    {
        cover.add(macro, &e);
    }

    instrCounts += e.getExps().size();
    for (auto exp : e.getExps())
    {
        if (exp->isCommentExp())
        {
            --instrCounts;
        }
        else
        {
            cover.add(macro, exp);
            exp->accept(*this);
        }
    }
}

void InstrumentVisitor::visit(ast::IfExp & e)
{
    ++branchesCount;
    e.getThen().accept(*this);
    if (e.hasElse())
    {
        ++branchesCount;
        if (e.getElse().isCommentExp())
        {
            cover.add(macro, &e.getElse());
        }
        else
        {
            e.getElse().accept(*this);
        }
    }
}

void InstrumentVisitor::visit(ast::WhileExp & e)
{
    ++branchesCount;
    e.getBody().accept(*this);
}

void InstrumentVisitor::visit(ast::ForExp & e)
{
    ++branchesCount;
    e.getBody().accept(*this);
}

void InstrumentVisitor::visit(ast::TryCatchExp & e)
{
    branchesCount += 2;
    e.getTry().accept(*this);
    e.getCatch().accept(*this);
}

void InstrumentVisitor::visit(ast::SelectExp & e)
{
    ast::exps_t cases = e.getCases();
    branchesCount += cases.size();
    for (auto exp : cases)
    {
        exp->accept(*this);
    }
    if (e.hasDefault())
    {
        ++branchesCount;
        e.getDefaultCase()->accept(*this);
    }
}

void InstrumentVisitor::visit(ast::CaseExp & e)
{
    cover.add(macro, e.getTest());
    ++instrCounts;
    e.getBody()->accept(*this);
}

void InstrumentVisitor::visit(ast::FunctionDec & e)
{
    types::Macro * pMacro = e.getMacro();
    if (!pMacro)
    {
        std::list<symbol::Variable *> * pVarList = new std::list<symbol::Variable *>();
        const ast::exps_t & vars = e.getArgs().getVars();
        for (const auto var : vars)
        {
            pVarList->push_back(var->getAs<ast::SimpleVar>()->getStack());
        }

        //get output parameters list
        std::list<symbol::Variable * > * pRetList = new std::list<symbol::Variable *>();
        const ast::exps_t & rets = e.getReturns().getVars();
        for (const auto ret : rets)
        {
            pRetList->push_back(ret->getAs<ast::SimpleVar>()->getStack());
        }

        pMacro = new types::Macro(e.getSymbol().getName(), *pVarList, *pRetList, static_cast<ast::SeqExp &>(e.getBody()), "script");
        pMacro->setLines(e.getLocation().first_line, e.getLocation().last_line);
        //pMacro->setFirstLine(e.getLocation().first_line);
        e.setMacro(pMacro);
    }

    inners.push_back(pMacro);
}
}