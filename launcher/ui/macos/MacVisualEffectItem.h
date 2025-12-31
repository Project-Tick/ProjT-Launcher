#pragma once

#include <QQuickItem>

#ifdef Q_OS_MACOS

class MacVisualEffectItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(Material material READ material WRITE setMaterial NOTIFY materialChanged)
    Q_PROPERTY(BlendingMode blendingMode READ blendingMode WRITE setBlendingMode NOTIFY blendingModeChanged)
    QML_ELEMENT

   public:
    enum Material {
        Titlebar = 0,
        Selection = 1,
        Menu = 2,
        Popover = 3,
        Sidebar = 4,
        HeaderView = 5,
        Sheet = 6,
        WindowBackground = 7,
        HUDWindow = 8,
        FullScreenUI = 9,
        ToolTip = 10,
        ContentBackground = 11,
        UnderWindowBackground = 12,
        UnderPageBackground = 13
    };
    Q_ENUM(Material)

    enum BlendingMode { BehindWindow = 0, WithinWindow = 1 };
    Q_ENUM(BlendingMode)

    explicit MacVisualEffectItem(QQuickItem* parent = nullptr);
    ~MacVisualEffectItem() override;

    Material material() const { return m_material; }
    void setMaterial(Material material);

    BlendingMode blendingMode() const { return m_blendingMode; }
    void setBlendingMode(BlendingMode mode);

   signals:
    void materialChanged();
    void blendingModeChanged();

   protected:
    void componentComplete() override;
    void itemChange(ItemChange change, const ItemChangeData& value) override;

   private:
    void updateEffect();
    void* m_effectView = nullptr;  // NSVisualEffectView*
    Material m_material = Sidebar;
    BlendingMode m_blendingMode = BehindWindow;
};

#endif  // Q_OS_MACOS
