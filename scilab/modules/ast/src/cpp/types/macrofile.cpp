/*
*  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
*  Copyright (C) 2009-2010 - DIGITEO - Bruno JOFRET
*  Copyright (C) 2009-2009 - DIGITEO - Antoine ELIAS
*
*  This file must be used under the terms of the CeCILL.
*  This source file is licensed as described in the file COPYING, which
*  you should have received as part of this distribution.  The terms
*  are also available at
*  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
*
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include "macrofile.hxx"
#include "context.hxx"
#include "localization.h"
#include "scilabWrite.hxx"
#include "parser.hxx"
#include "configvariable.hxx"
#include "deserializervisitor.hxx"
#include "scilabWrite.hxx"

namespace types
{
MacroFile::MacroFile(std::wstring _stName, std::wstring _stPath, std::wstring _stModule) :
    Callable(), m_stPath(_stPath), m_pMacro(NULL)
{
    setName(_stName);
    setModule(_stModule);
}

MacroFile::~MacroFile()
{
    if (m_pMacro)
    {
        delete m_pMacro;
    }
}

InternalType* MacroFile::clone()
{
    IncreaseRef();
    return this;
}

void MacroFile::whoAmI()
{
    std::cout << "types::MacroFile";
}

bool MacroFile::toString(std::wostringstream& ostr)
{
    ostr << L"FIXME : Implement MacroFile::toString" << std::endl;
    return true;
}

Callable::ReturnValue MacroFile::call(typed_list &in, optional_list &opt, int _iRetCount, typed_list &out, ast::ConstVisitor* execFunc)
{
    parse();
    if (m_pMacro)
    {
        return m_pMacro->call(in, opt, _iRetCount, out, execFunc);
    }
    else
    {
        return Callable::Error;
    }
}

bool MacroFile::parse(void)
{
    if (m_pMacro == NULL)
    {
        //load file, only for the first call
        char* pstPath = wide_string_to_UTF8(m_stPath.c_str());
        std::ifstream f(pstPath, ios::in | ios::binary | ios::ate);
        FREE(pstPath);

        int size = (int)f.tellg();
        unsigned char* binAst = new unsigned char[size];
        f.seekg(0);
        f.read((char*)binAst, size);
        f.close();
        ast::DeserializeVisitor d(binAst);
        ast::Exp* tree = d.deserialize();
        delete[] binAst;

        //find FunctionDec
        ast::FunctionDec* pFD = NULL;

        ast::exps_t::iterator j;
        ast::exps_t LExp = tree->getAs<ast::SeqExp>()->getExps();

        for (j = LExp.begin() ; j != LExp.end() ; j++)
        {
            if ((*j)->isFunctionDec() == false)
            {
                continue;
            }

            pFD = (*j)->getAs<ast::FunctionDec>();
            if (pFD) // &&	pFD->getName() == m_stName
            {
                symbol::Context* pContext = symbol::Context::getInstance();
                InternalType* pFunc = pContext->getFunction(pFD->getSymbol());
                if (pFunc && pFunc->isMacroFile())
                {
                    MacroFile* pMacro = pContext->getFunction(pFD->getSymbol())->getAs<MacroFile>();
                    if (pMacro->m_pMacro == NULL)
                    {

                        //get input parameters list
                        std::list<symbol::Variable*> *pVarList = new std::list<symbol::Variable*>();
                        ast::ArrayListVar *pListVar = pFD->getArgs().getAs<ast::ArrayListVar>();
                        ast::exps_t vars = pListVar->getVars();
                        for (ast::exps_t::const_iterator it = vars.begin(), itEnd = vars.end() ; it != itEnd ; ++it)
                        {
                            pVarList->push_back((*it)->getAs<ast::SimpleVar>()->getStack());
                        }

                        //get output parameters list
                        std::list<symbol::Variable*> *pRetList = new std::list<symbol::Variable*>();
                        ast::ArrayListVar *pListRet = pFD->getReturns().getAs<ast::ArrayListVar>();
                        ast::exps_t recs = pListRet->getVars();
                        for (ast::exps_t::const_iterator it = recs.begin(), itEnd = recs.end(); it != itEnd ; ++it)
                        {
                            pRetList->push_back((*it)->getAs<ast::SimpleVar>()->getStack());
                        }

                        pMacro->m_pMacro = new Macro(m_wstName, *pVarList, *pRetList, (ast::SeqExp&)pFD->getBody(), m_wstModule);
                        pMacro->setFirstLine(pFD->getLocation().first_line);
                    }
                }
            }
            delete *j;
        }

        ((ast::SeqExp*)tree)->clearExps();
        delete tree;

    }
    return true;
}

int MacroFile::getNbInputArgument(void)
{
    return getMacro()->getNbInputArgument();
}

int MacroFile::getNbOutputArgument(void)
{
    return getMacro()->getNbOutputArgument();
}

Macro* MacroFile::getMacro(void)
{
    parse();
    return m_pMacro;
}

void MacroFile::setFirstLine(int _iLine)
{
    getMacro()->setFirstLine(_iLine);
}

bool MacroFile::operator==(const InternalType& it)
{
    if (const_cast<InternalType &>(it).isMacro() == false && const_cast<InternalType &>(it).isMacroFile() == false)
    {
        return false;
    }

    Macro* pL = getMacro();
    Macro* pR = NULL;

    if (const_cast<InternalType &>(it).isMacroFile())
    {
        MacroFile* pMF = const_cast<InternalType &>(it).getAs<types::MacroFile>();
        pR = pMF->getMacro();
    }
    else
    {
        pR = const_cast<InternalType &>(it).getAs<types::Macro>();
    }

    return (*pL == *pR);
}
}