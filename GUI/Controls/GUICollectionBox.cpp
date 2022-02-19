#include "GUI.h"
#include "GUICollectionBox.h"

namespace RTE {

	const std::string_view GUICollectionBox::c_ControlType = "COLLECTIONBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Create(const std::string_view &name, int posX, int posY, int width, int height) {
		GUIControl::Create(name, posX, posY, (width > 0) ? std::max(width, m_MinWidth) : m_DefaultWidth, (height > 0) ? std::max(height, m_MinHeight) : m_DefaultHeight);

		m_Properties.AddProperty("DrawBackground", true);
		m_Properties.AddProperty("DrawType", DrawType::Color);
		m_Properties.AddProperty("DrawColor", 0);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Create(GUIProperties *reference) {
		GUIControl::Create(reference);

		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		m_Properties.AddProperty("DrawBackground", reference->GetPropertyValue<std::string>("DrawBackground"));
		m_Properties.AddProperty("DrawType", (reference->GetPropertyValue<std::string>("DrawType") == "Color") ? DrawType::Color : DrawType::Image);
		m_Properties.AddProperty("DrawColor", reference->GetPropertyValue<int>("DrawColor"));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Move(int newPosX, int newPosY) {
		int relPosX = newPosX - m_X;
		int relPosY = newPosY - m_Y;
		m_X = newPosX;
		m_Y = newPosY;

		for (GUIControl *childControl : m_Children) {
			int childPosX;
			int childPosY;
			childControl->GetControlRect(&childPosX, &childPosY, nullptr, nullptr);
			childControl->Move(childPosX + relPosX, childPosY + relPosY);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Resize(int Width, int Height) {
		int oldWidth = m_Width;
		int oldHeight = m_Height;
		m_Width = Width;
		m_Height = Height;

		for (GUIControl *childControl : m_Children) {
			int childAnchor = childControl->GetAnchor();

			int childPosX;
			int childPosY;
			int childWidth;
			int childHeight;
			childControl->GetControlRect(&childPosX, &childPosY, &childWidth, &childHeight);

			int destPosX = childPosX;
			int destPosY = childPosY;
			int destWidth = childWidth;
			int destHeight = childHeight;

			if ((childAnchor & GUIControl::Anchor::AnchorRight) && !(childAnchor & GUIControl::Anchor::AnchorLeft)) { destPosX = m_Width - (oldWidth - (destPosX - m_X)) + m_X; }
			if ((childAnchor & GUIControl::Anchor::AnchorBottom) && !(childAnchor & GUIControl::Anchor::AnchorTop)) { destPosY = m_Height - (oldHeight - (destPosY - m_Y)) + m_Y; }
			if (destPosX != childPosX || destPosY != childPosX) { childControl->Move(destPosX, destPosY); }

			childPosX -= m_X;
			childPosY -= m_Y;

			if (childAnchor & GUIControl::Anchor::AnchorLeft && childAnchor & GUIControl::Anchor::AnchorRight) { destWidth = (m_Width - (oldWidth - (childPosX + childWidth))) - destPosX; }
			if (childAnchor & GUIControl::Anchor::AnchorTop && childAnchor & GUIControl::Anchor::AnchorBottom) { destHeight = (m_Height - (oldHeight - (childPosY + childHeight))) - destPosY; }
			if (destWidth != childWidth || destHeight != childHeight) { childControl->Resize(destWidth, destHeight); }
		}
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::BuildBitmap() {
		m_DrawBitmap.reset(m_Skin->CreateBitmap(m_Width, m_Height));
		m_Skin->BuildStandardRect(m_DrawBitmap.get(), "CollectionBox_Panel", 0, 0, m_Width, m_Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::ChangeSkin(GUISkin *newSkin) {
		GUIControl::ChangeSkin(newSkin);
		BuildBitmap();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUICollectionBox::Draw(GUIScreen *targetScreen) {
		if (GetDrawBackground()) {
			targetScreen->GetBitmap()->SetClipRect(GetRect());
			if (GetDrawType() == DrawType::Color) {
				targetScreen->GetBitmap()->DrawRectangle(m_X, m_Y, m_Width, m_Height, m_Skin->ConvertColor(GetDrawColor(), targetScreen->GetBitmap()->GetColorDepth()), true);
			} else {
				if (m_DrawBitmap) { m_DrawBitmap->DrawTrans(targetScreen->GetBitmap(), m_X, m_Y, nullptr); }
			}
			targetScreen->GetBitmap()->SetClipRect(nullptr);
		}
		GUIControl::Draw(targetScreen);
	}
}