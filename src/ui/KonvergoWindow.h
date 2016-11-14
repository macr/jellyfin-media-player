#ifndef KONVERGOWINDOW_H
#define KONVERGOWINDOW_H

#include <QQuickWindow>
#include <QEvent>
#include <settings/SettingsComponent.h>


// This controls how big the web view will zoom using semantic zoom
// over a specific number of pixels and we run out of space for on screen
// tiles in chromium. This only happens on OSX since on other platforms
// we can use the GPU to transfer tiles directly but we set the limit on all platforms
// to keep it consistent.
//
// See more discussion in: https://github.com/plexinc/plex-media-player/issues/10
// The number of pixels here are REAL pixels, the code in webview.qml will compensate
// for a higher DevicePixelRatio
//
#define WEBUI_MAX_HEIGHT 1440.0
#define WEBUI_SIZE QSize(1280, 720)
#define WINDOWW_MIN_SIZE QSize(426, 240)

///////////////////////////////////////////////////////////////////////////////////////////////////
class KonvergoWindow : public QQuickWindow
{
  Q_OBJECT

  Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenSwitched)
  Q_PROPERTY(bool showDebugLayer MEMBER m_debugLayer NOTIFY debugLayerChanged)
  Q_PROPERTY(QString debugInfo MEMBER m_debugInfo NOTIFY debugInfoChanged)
  Q_PROPERTY(QString videoInfo MEMBER m_videoInfo NOTIFY debugInfoChanged)
  Q_PROPERTY(qreal webScale READ webScale NOTIFY webScaleChanged)
  Q_PROPERTY(qreal webHeightMax READ webHeightMax NOTIFY webScaleChanged)
  Q_PROPERTY(QSize webUISize READ webUISize NOTIFY webScaleChanged)
  Q_PROPERTY(qreal windowScale READ windowScale NOTIFY webScaleChanged)
  Q_PROPERTY(QSize windowMinSize READ windowMinSize NOTIFY webScaleChanged)
  Q_PROPERTY(bool alwaysOnTop READ isAlwaysOnTop WRITE setAlwaysOnTop)
  Q_PROPERTY(bool webDesktopMode MEMBER m_webDesktopMode NOTIFY webDesktopModeChanged)
  Q_PROPERTY(QString webUrl READ webUrl NOTIFY webUrlChanged)

public:
  static void RegisterClass();

  explicit KonvergoWindow(QWindow* parent = nullptr);
  ~KonvergoWindow() override;

  bool isFullScreen()
  {
    return ((flags() & Qt::FramelessWindowHint) || (visibility() == QWindow::FullScreen));
  }

  Q_INVOKABLE void setFullScreen(bool enable);

  bool isAlwaysOnTop()
  {
    Qt::WindowFlags forceOnTopFlags = Qt::WindowStaysOnTopHint;
#ifdef Q_OS_LINUX
    forceOnTopFlags = forceOnTopFlags | Qt::X11BypassWindowManagerHint;
#endif
    return (flags() & forceOnTopFlags);
  }

  void setAlwaysOnTop(bool enable);

  Q_SLOT void otherAppFocus()
  {
    setWindowState((Qt::WindowState)((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive));
    raise();
  }

  Q_SLOT void toggleDebug();

  Q_SLOT void toggleFullscreen()
  {
    setFullScreen(!isFullScreen());
  }

  Q_SLOT void toggleAlwaysOnTop()
  {
    setAlwaysOnTop(!isAlwaysOnTop());
  }

  Q_SLOT void reloadWeb()
  {
    emit reloadWebClient();
  }

  qreal windowScale() { return CalculateScale(size()); }
  qreal webScale() { return CalculateWebScale(size(), devicePixelRatio()); }
  qreal webHeightMax() { return WEBUI_MAX_HEIGHT; }
  QSize webUISize() { return WEBUI_SIZE; }
  QSize windowMinSize() { return WINDOWW_MIN_SIZE; }
  static qreal CalculateScale(const QSize& size);
  static qreal CalculateWebScale(const QSize& size, qreal devicePixelRatio);
  QString webUrl();

Q_SIGNALS:
  void fullScreenSwitched();
  void webScaleChanged();
  void enableVideoWindowSignal();
  void debugLayerChanged();
  void debugInfoChanged();
  void reloadWebClient();
  void webDesktopModeChanged();
  void webUrlChanged();

protected:
  void focusOutEvent(QFocusEvent* ev) override;
  void resizeEvent(QResizeEvent* event) override;

private slots:
  void closingWindow();
  void enableVideoWindow();
  void onVisibilityChanged(QWindow::Visibility visibility);
  void updateMainSectionSettings(const QVariantMap& values);
  void updateWindowState(bool saveGeo = true);
  void onScreenCountChanged(int newCount);
  void updateDebugInfo();
  void playerWindowVisible(bool visible);
  void showUpdateDialog();

private:
  void notifyScale(const QSize& size);
  void saveGeometry();
  QRect loadGeometry();
  QRect loadGeometryRect();
  bool fitsInScreens(const QRect& rc);
  QScreen* loadLastScreen();

  bool m_debugLayer;
  qreal m_lastScale;
  QTimer* m_infoTimer;
  QString m_debugInfo, m_systemDebugInfo, m_videoInfo;
  int m_ignoreFullscreenSettingsChange;
  bool m_webDesktopMode;
  bool m_showedUpdateDialog;
};

#endif // KONVERGOWINDOW_H
