/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MobileConnection.h"
#include "nsIDOMDOMRequest.h"
#include "nsIDOMClassInfo.h"
#include "nsDOMEvent.h"
#include "nsIDOMUSSDReceivedEvent.h"
#include "nsIDOMDataErrorEvent.h"
#include "nsIDOMCFStateChangeEvent.h"
#include "nsIDOMICCCardLockErrorEvent.h"
#include "GeneratedEvents.h"

#include "nsContentUtils.h"
#include "nsJSUtils.h"
#include "nsJSON.h"
#include "jsapi.h"
#include "mozilla/Services.h"
#include "IccManager.h"

#define NS_RILCONTENTHELPER_CONTRACTID "@mozilla.org/ril/content-helper;1"

using namespace mozilla::dom::network;

class MobileConnection::Listener : public nsIMobileConnectionListener
{
  MobileConnection* mMobileConnection;

public:
  NS_DECL_ISUPPORTS
  NS_FORWARD_SAFE_NSIMOBILECONNECTIONLISTENER(mMobileConnection)

  Listener(MobileConnection* aMobileConnection)
    : mMobileConnection(aMobileConnection)
  {
    MOZ_ASSERT(mMobileConnection);
  }

  void Disconnect()
  {
    MOZ_ASSERT(mMobileConnection);
    mMobileConnection = nullptr;
  }
};

NS_IMPL_ISUPPORTS1(MobileConnection::Listener, nsIMobileConnectionListener)

DOMCI_DATA(MozMobileConnection, MobileConnection)

NS_IMPL_CYCLE_COLLECTION_TRAVERSE_BEGIN_INHERITED(MobileConnection,
                                                  nsDOMEventTargetHelper)
  // Don't traverse mListener because it doesn't keep any reference to
  // MobileConnection but a raw pointer instead. Neither does mProvider because
  // it's an xpcom service and is only released at shutting down.
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE(mIccManager)
NS_IMPL_CYCLE_COLLECTION_TRAVERSE_END

NS_IMPL_CYCLE_COLLECTION_UNLINK_BEGIN_INHERITED(MobileConnection,
                                                nsDOMEventTargetHelper)
  NS_IMPL_CYCLE_COLLECTION_UNLINK(mIccManager)
NS_IMPL_CYCLE_COLLECTION_UNLINK_END

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION_INHERITED(MobileConnection)
  NS_INTERFACE_MAP_ENTRY(nsIDOMMozMobileConnection)
  NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO(MozMobileConnection)
NS_INTERFACE_MAP_END_INHERITING(nsDOMEventTargetHelper)

NS_IMPL_ADDREF_INHERITED(MobileConnection, nsDOMEventTargetHelper)
NS_IMPL_RELEASE_INHERITED(MobileConnection, nsDOMEventTargetHelper)

NS_IMPL_EVENT_HANDLER(MobileConnection, cardstatechange)
NS_IMPL_EVENT_HANDLER(MobileConnection, iccinfochange)
NS_IMPL_EVENT_HANDLER(MobileConnection, voicechange)
NS_IMPL_EVENT_HANDLER(MobileConnection, datachange)
NS_IMPL_EVENT_HANDLER(MobileConnection, ussdreceived)
NS_IMPL_EVENT_HANDLER(MobileConnection, dataerror)
NS_IMPL_EVENT_HANDLER(MobileConnection, icccardlockerror)
NS_IMPL_EVENT_HANDLER(MobileConnection, cfstatechange)

MobileConnection::MobileConnection()
{
  mProvider = do_GetService(NS_RILCONTENTHELPER_CONTRACTID);

  // Not being able to acquire the provider isn't fatal since we check
  // for it explicitly below.
  if (!mProvider) {
    NS_WARNING("Could not acquire nsIMobileConnectionProvider!");
    return;
  }

  mListener = new Listener(this);
  DebugOnly<nsresult> rv = mProvider->RegisterMobileConnectionMsg(mListener);
  NS_WARN_IF_FALSE(NS_SUCCEEDED(rv),
                   "Failed registering mobile connection messages with provider");
}

void
MobileConnection::Init(nsPIDOMWindow* aWindow)
{
  BindToOwner(aWindow);

  mIccManager = new icc::IccManager();
  mIccManager->Init(aWindow);
}

void
MobileConnection::Shutdown()
{
  if (mProvider && mListener) {
    mListener->Disconnect();
    mProvider->UnregisterMobileConnectionMsg(mListener);
    mProvider = nullptr;
    mListener = nullptr;
  }

  if (mIccManager) {
    mIccManager->Shutdown();
    mIccManager = nullptr;
  }
}

// nsIDOMMozMobileConnection

NS_IMETHODIMP
MobileConnection::GetCardState(nsAString& cardState)
{
  if (!mProvider) {
    cardState.SetIsVoid(true);
    return NS_OK;
  }
  return mProvider->GetCardState(cardState);
}

NS_IMETHODIMP
MobileConnection::GetIccInfo(nsIDOMMozMobileICCInfo** aIccInfo)
{
  if (!mProvider) {
    *aIccInfo = nullptr;
    return NS_OK;
  }
  return mProvider->GetIccInfo(aIccInfo);
}

NS_IMETHODIMP
MobileConnection::GetVoice(nsIDOMMozMobileConnectionInfo** voice)
{
  if (!mProvider) {
    *voice = nullptr;
    return NS_OK;
  }
  return mProvider->GetVoiceConnectionInfo(voice);
}

NS_IMETHODIMP
MobileConnection::GetData(nsIDOMMozMobileConnectionInfo** data)
{
  if (!mProvider) {
    *data = nullptr;
    return NS_OK;
  }
  return mProvider->GetDataConnectionInfo(data);
}

NS_IMETHODIMP
MobileConnection::GetNetworkSelectionMode(nsAString& networkSelectionMode)
{
  if (!mProvider) {
    networkSelectionMode.SetIsVoid(true);
    return NS_OK;
  }
  return mProvider->GetNetworkSelectionMode(networkSelectionMode);
}

NS_IMETHODIMP
MobileConnection::GetIcc(nsIDOMMozIccManager** aIcc)
{
  NS_IF_ADDREF(*aIcc = mIccManager);
  return NS_OK;
}

NS_IMETHODIMP
MobileConnection::GetNetworks(nsIDOMDOMRequest** request)
{
  *request = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->GetNetworks(GetOwner(), request);
}

NS_IMETHODIMP
MobileConnection::SelectNetwork(nsIDOMMozMobileNetworkInfo* network, nsIDOMDOMRequest** request)
{
  *request = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->SelectNetwork(GetOwner(), network, request);
}

NS_IMETHODIMP
MobileConnection::SelectNetworkAutomatically(nsIDOMDOMRequest** request)
{
  *request = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->SelectNetworkAutomatically(GetOwner(), request);
}

NS_IMETHODIMP
MobileConnection::GetCardLock(const nsAString& aLockType, nsIDOMDOMRequest** aDomRequest)
{
  *aDomRequest = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->GetCardLock(GetOwner(), aLockType, aDomRequest);
}

NS_IMETHODIMP
MobileConnection::UnlockCardLock(const jsval& aInfo, nsIDOMDOMRequest** aDomRequest)
{
  *aDomRequest = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->UnlockCardLock(GetOwner(), aInfo, aDomRequest);
}

NS_IMETHODIMP
MobileConnection::SetCardLock(const jsval& aInfo, nsIDOMDOMRequest** aDomRequest)
{
  *aDomRequest = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->SetCardLock(GetOwner(), aInfo, aDomRequest);
}

NS_IMETHODIMP
MobileConnection::SendMMI(const nsAString& aMMIString,
                          nsIDOMDOMRequest** request)
{
  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->SendMMI(GetOwner(), aMMIString, request);
}

NS_IMETHODIMP
MobileConnection::CancelMMI(nsIDOMDOMRequest** request)
{
  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->CancelMMI(GetOwner(), request);
}

NS_IMETHODIMP
MobileConnection::GetCallForwardingOption(uint16_t aReason,
                                          nsIDOMDOMRequest** aRequest)
{
  *aRequest = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->GetCallForwardingOption(GetOwner(), aReason, aRequest);
}

NS_IMETHODIMP
MobileConnection::SetCallForwardingOption(nsIDOMMozMobileCFInfo* aCFInfo,
                                          nsIDOMDOMRequest** aRequest)
{
  *aRequest = nullptr;

  if (!mProvider) {
    return NS_ERROR_FAILURE;
  }

  return mProvider->SetCallForwardingOption(GetOwner(), aCFInfo, aRequest);
}

// nsIMobileConnectionListener

NS_IMETHODIMP
MobileConnection::NotifyVoiceChanged()
{
  return DispatchTrustedEvent(NS_LITERAL_STRING("voicechange"));
}

NS_IMETHODIMP
MobileConnection::NotifyDataChanged()
{
  return DispatchTrustedEvent(NS_LITERAL_STRING("datachange"));
}

NS_IMETHODIMP
MobileConnection::NotifyCardStateChanged()
{
  return DispatchTrustedEvent(NS_LITERAL_STRING("cardstatechange"));
}

NS_IMETHODIMP
MobileConnection::NotifyIccInfoChanged()
{
  return DispatchTrustedEvent(NS_LITERAL_STRING("iccinfochange"));
}

NS_IMETHODIMP
MobileConnection::NotifyUssdReceived(const nsAString& aMessage,
                                     bool aSessionEnded)
{
  nsCOMPtr<nsIDOMEvent> event;
  NS_NewDOMUSSDReceivedEvent(getter_AddRefs(event), this, nullptr, nullptr);

  nsCOMPtr<nsIDOMUSSDReceivedEvent> ce = do_QueryInterface(event);
  nsresult rv = ce->InitUSSDReceivedEvent(NS_LITERAL_STRING("ussdreceived"),
                                          false, false,
                                          aMessage, aSessionEnded);
  NS_ENSURE_SUCCESS(rv, rv);

  return DispatchTrustedEvent(ce);
}

NS_IMETHODIMP
MobileConnection::NotifyDataError(const nsAString& aMessage)
{
  nsCOMPtr<nsIDOMEvent> event;
  NS_NewDOMDataErrorEvent(getter_AddRefs(event), this, nullptr, nullptr);

  nsCOMPtr<nsIDOMDataErrorEvent> ce = do_QueryInterface(event);
  nsresult rv = ce->InitDataErrorEvent(NS_LITERAL_STRING("dataerror"),
                                       false, false, aMessage);
  NS_ENSURE_SUCCESS(rv, rv);

  return DispatchTrustedEvent(ce);
}

NS_IMETHODIMP
MobileConnection::NotifyIccCardLockError(const nsAString& aLockType,
                                         uint32_t aRetryCount)
{
  nsCOMPtr<nsIDOMEvent> event;
  NS_NewDOMICCCardLockErrorEvent(getter_AddRefs(event), this, nullptr, nullptr);

  nsCOMPtr<nsIDOMICCCardLockErrorEvent> ce = do_QueryInterface(event);
  nsresult rv =
    ce->InitICCCardLockErrorEvent(NS_LITERAL_STRING("icccardlockerror"),
                                  false, false, aLockType, aRetryCount);
  NS_ENSURE_SUCCESS(rv, rv);

  return DispatchTrustedEvent(ce);
}

NS_IMETHODIMP
MobileConnection::NotifyCFStateChange(bool aSuccess,
                                      unsigned short aAction,
                                      unsigned short aReason,
                                      const nsAString& aNumber,
                                      unsigned short aSeconds,
                                      unsigned short aServiceClass)
{
  nsCOMPtr<nsIDOMEvent> event;
  NS_NewDOMCFStateChangeEvent(getter_AddRefs(event), this, nullptr, nullptr);

  nsCOMPtr<nsIDOMCFStateChangeEvent> ce = do_QueryInterface(event);
  nsresult rv = ce->InitCFStateChangeEvent(NS_LITERAL_STRING("cfstatechange"),
                                           false, false,
                                           aSuccess, aAction, aReason, aNumber,
                                           aSeconds, aServiceClass);
  NS_ENSURE_SUCCESS(rv, rv);

  return DispatchTrustedEvent(ce);
}
