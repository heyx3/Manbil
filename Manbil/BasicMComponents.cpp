/*#include "BasicMComponents.h"

bool CameraComponent::ReceiveMessage(int message, void * pMsgData)
{
	MessageArgs_CanComponentRenderLayers * argsCanRender;
	MessageArgs_AddOrRemoveRenderLayer * argsAddRemove;
	switch (message)
	{
		case (int)Messages::MESSAGE_CAN_RENDER_ANY_RENDER_LAYERS:
			argsCanRender = (MessageArgs_CanComponentRenderLayers*)pMsgData;
			argsCanRender->OutResult = CanRenderAnyLayers(argsCanRender->LayersToCheckFor);
			break;

		case (int)Messages::MESSAGE_CAN_RENDER_ALL_RENDER_LAYERS:
			argsCanRender = (MessageArgs_CanComponentRenderLayers*)pMsgData;
			argsCanRender->OutResult = CanRenderAllLayers(argsCanRender->LayersToCheckFor);
			break;

		case (int)Messages::MESSAGE_ADD_RENDER_LAYERS:
			argsAddRemove = (MessageArgs_AddOrRemoveRenderLayer*)pMsgData;
			AddRenderLayers(*argsAddRemove);
			break;

		case (int)Messages::MESSAGE_REMOVE_RENDER_LAYERS:
			argsAddRemove = (MessageArgs_AddOrRemoveRenderLayer*)pMsgData;
			RemoveRenderLayers(*argsAddRemove);
			break;

		default: return MComponent::ReceiveMessage(message, pMsgData);
	}

	return true;
}

bool RenderableComponent::ReceiveMessage(int message, void * pMsgData)
{
	MessageArgs_AddOrRemoveRenderLayer * argsAddRemove;
	MessageArgs_IsComponentOnAnyRenderLayers * argsIsComponentOn;

	switch (message)
	{
		case (int)Messages::MESSAGE_RENDER_GAME:
			Render(*(MCameraInfo*)pMsgData);
			break;

		case (int)Messages::MESSAGE_GET_IS_ON_ANY_RENDER_LAYERS:
			argsIsComponentOn = (MessageArgs_IsComponentOnAnyRenderLayers*)pMsgData;
			argsIsComponentOn->OutResult = IsOnAnyRenderLayers(argsIsComponentOn->LayersToCheckFor);
			break;

		case (int)Messages::MESSAGE_ADD_RENDER_LAYERS:
			argsAddRemove = (MessageArgs_AddOrRemoveRenderLayer*)pMsgData;
			AddRenderLayers(*argsAddRemove);
			break;

		case (int)Messages::MESSAGE_REMOVE_RENDER_LAYERS:
			argsAddRemove = (MessageArgs_AddOrRemoveRenderLayer*)pMsgData;
			RemoveRenderLayers(*argsAddRemove);
			break;

		default: return MComponent::ReceiveMessage(message, pMsgData);
	}

	return true;
}*/