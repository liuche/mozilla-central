/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/CDATASection.h"
#include "mozilla/dom/CDATASectionBinding.h"

nsresult
NS_NewXMLCDATASection(nsIContent** aInstancePtrResult,
                      nsNodeInfoManager *aNodeInfoManager)
{
  using mozilla::dom::CDATASection;

  NS_PRECONDITION(aNodeInfoManager, "Missing nodeinfo manager");

  *aInstancePtrResult = nullptr;

  nsCOMPtr<nsINodeInfo> ni;
  ni = aNodeInfoManager->GetNodeInfo(nsGkAtoms::cdataTagName,
                                     nullptr, kNameSpaceID_None,
                                     nsIDOMNode::CDATA_SECTION_NODE);
  NS_ENSURE_TRUE(ni, NS_ERROR_OUT_OF_MEMORY);

  CDATASection *instance = new CDATASection(ni.forget());
  if (!instance) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  NS_ADDREF(*aInstancePtrResult = instance);

  return NS_OK;
}

namespace mozilla {
namespace dom {

CDATASection::~CDATASection()
{
}

NS_IMPL_ISUPPORTS_INHERITED4(CDATASection, nsGenericDOMDataNode, nsIDOMNode,
                             nsIDOMCharacterData, nsIDOMText,
                             nsIDOMCDATASection)

JSObject*
CDATASection::WrapNode(JSContext *aCx, JSObject *aScope)
{
  return CDATASectionBinding::Wrap(aCx, aScope, this);
}

bool
CDATASection::IsNodeOfType(uint32_t aFlags) const
{
  return !(aFlags & ~(eCONTENT | eTEXT | eDATA_NODE));
}

nsGenericDOMDataNode*
CDATASection::CloneDataNode(nsINodeInfo *aNodeInfo, bool aCloneText) const
{
  nsCOMPtr<nsINodeInfo> ni = aNodeInfo;
  CDATASection *it = new CDATASection(ni.forget());
  if (it && aCloneText) {
    it->mText = mText;
  }

  return it;
}

#ifdef DEBUG
void
CDATASection::List(FILE* out, int32_t aIndent) const
{
  int32_t index;
  for (index = aIndent; --index >= 0; ) fputs("  ", out);

  fprintf(out, "CDATASection refcount=%d<", mRefCnt.get());

  nsAutoString tmp;
  ToCString(tmp, 0, mText.GetLength());
  fputs(NS_LossyConvertUTF16toASCII(tmp).get(), out);

  fputs(">\n", out);
}

void
CDATASection::DumpContent(FILE* out, int32_t aIndent,
                               bool aDumpAll) const {
}
#endif

} // namespace mozilla
} // namespace dom
