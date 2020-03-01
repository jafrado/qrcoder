

#include <QtGui>
#include <QWidget>
#include <QMainWindow>
#include <qrencode.h>

static const size_t DEFAULT_ZOOM_SIZE = 10;
static const size_t MAX_ZOOM_SIZE = 100;

enum QR_EMode { Numeric = QR_MODE_NUM, AlphaNumeric = QR_MODE_AN, DataMode = QR_MODE_8, KanjiMode = QR_MODE_KANJI, Structure = QR_MODE_STRUCTURE, ECIMode = QR_MODE_ECI, FNC1FMode = QR_MODE_FNC1FIRST, FNC1SMode = QR_MODE_FNC1SECOND };
static const QString QR_EM_MODESTR[] = { "Numeric", "Alpha-Numeric", "8-Bit Data", "Kanji", "Structure", "ECI Mode", "FNC1 1st", "FNC1 2nd" };

//Error correction mode
enum QR_ECMode { Level0 = QR_ECLEVEL_L, Level1 = QR_ECLEVEL_M, Level2 = QR_ECLEVEL_Q, Level3 = QR_ECLEVEL_H };
static const QString QR_EC_MODESTR[] = { "Low (7%)", "Medium (14%)", "High (21%)", "Highest (30%)" };

class  QRCode : public QWidget{

	Q_OBJECT

	QRcode* qrcode;
	char* pixmap;
	int encMode;
	int ecMode;

public:
    size_t width;
    size_t scale;
    explicit QRCode(QWindow* parent = nullptr) : qrcode(nullptr), pixmap(nullptr), encMode(QR_EMode::DataMode), ecMode(QR_ECMode::Level0), width(0), scale(DEFAULT_ZOOM_SIZE)	{}
    ~QRCode() {}
    void setScale(size_t scale) { this->scale = scale;  if (this->scale > MAX_ZOOM_SIZE) this->scale = MAX_ZOOM_SIZE; }
	void generateQRCode(QString text);
	void generatePixMap(void);
	int getTrueWidth(void) { return (qrcode != nullptr ? qrcode->width : 0); }
	void setEncoding(int mode) { encMode = mode; }
	void setEC(int mode) { ecMode = mode; }

protected:
	void paintEvent(QPaintEvent* event) override;
		
private:
	QBackingStore* backingStore;
};


namespace Ui {
	class QRCoder;
}

class QRCoder : public QMainWindow {
	Q_OBJECT
	Ui::QRCoder* ui;
	QRCode* q;
    double screenDpi;
    double qrCodeDpi;
	QString filename;

public:
	explicit QRCoder(QWidget* parent = 0);
	~QRCoder();

private Q_SLOTS:
    void updateStatusLine(double size);
	void setZoomLevel(int level);
	void setScanDistance(int cm);
	void setText();
	void exitApplication();
	void ecChanged(int mode);
	void encChanged(int mode);
	void actionSave(void);
	void actionSaveAs(void);
	void actionPrint(void);
};
