/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the TransforMiiX XSLT processor.
 *
 * The Initial Developer of the Original Code is
 * Jonas Sicking.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Jonas Sicking <sicking@bigfoot.com>
 *   Peter Van der Beken <peterv@netscape.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef TRANSFRMX_NODESORTER_H
#define TRANSFRMX_NODESORTER_H

#include "baseutils.h"
#include "List.h"
#include "nsIAtom.h"

class Element;
class Expr;
class Node;
class NodeSet;
class txExecutionState;
class TxObject;
class txXPathResultComparator;
class txIEvalContext;
class txNodeSetContext;

/*
 * Sorts Nodes as specified by the W3C XSLT 1.0 Recommendation
 */

class txNodeSorter
{
public:
    txNodeSorter();
    ~txNodeSorter();

    nsresult addSortElement(Expr* aSelectExpr, Expr* aLangExpr,
                            Expr* aDataTypeExpr, Expr* aOrderExpr,
                            Expr* aCaseOrderExpr, txIEvalContext* aContext);
    nsresult sortNodeSet(NodeSet* aNodes, txExecutionState* aEs,
                         NodeSet** aResult);

private:
    struct SortData
    {
        txNodeSorter* mNodeSorter;
        txNodeSetContext* mContext;
        TxObject** mSortValues;
        nsresult mRv;
    };
    struct SortKey
    {
        Expr* mExpr;
        txXPathResultComparator* mComparator;
    };

    static int compareNodes(const void* aIndexA, const void* aIndexB,
                            void* aSortData);
    static PRBool calcSortValue(TxObject*& aSortValue, SortKey* aKey,
                                SortData* aSortData, PRUint32 aNodeIndex);
    txList mSortKeys;
    int mNKeys;
};

#endif
