#include "QRCoder.h"
#include "ui_QRCoder.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <iostream>


/* 
 * 1) your QR Code should have appropriate margin width.This width should be equal to about four data modules 
 * 2) always make sure there is enough color contrast between your QR Code and its background
 * 3) Use High EC for adding images
 * 4) QR Code Min Size = Scan Distance * No Rows/Columns / 250
 */


void QRCode::generatePixMap(void)
{
	if (pixmap != nullptr)
		delete pixmap;

	width = qrcode->width * scale;
	unsigned char* qrcode_data = qrcode->data;
	pixmap = new char[width * width];
	memset(pixmap, 0, width * width);

	/**
	 * From: https://github.com/fukuchi/libqrencode/blob/master/qrencode.h
	 * QRcode class.
	 * Symbol data is represented as an array contains width*width uchars.
	 * Each uchar represents a module (dot). If the less significant bit of
	 * the uchar is 1, the corresponding module is black. The other bits are
	 * meaningless for usual applications, but here its specification is described.
	 *
	 * @verbatim
	   MSB 76543210 LSB
		   |||||||`- 1=black/0=white
		   ||||||`-- 1=ecc/0=data code area
		   |||||`--- format information
		   ||||`---- version information
		   |||`----- timing pattern
		   ||`------ alignment pattern
		   |`------- finder pattern and separator
		   `-------- non-data modules (format, timing, etc.)
	   @endverbatim
	*/

	for (size_t y = 0; y < width; y += scale) {
		size_t y_index = y * width;
		for (size_t x = 0; x < width; x += scale) {
			int v = (*qrcode_data & 1) ? 1 : 0;
			for (size_t iy = 0; iy < scale; ++iy) {
				size_t iy_index = (y_index + iy * width);
				for (size_t ix = 0; ix < scale; ++ix) {
					pixmap[iy_index + (x + ix)] = v;
				}
			}
			++qrcode_data;
		}
	}
}

void QRCode::generateQRCode(QString text)
{
	if (text.length() <= 0)
		return;

	if (qrcode != nullptr) {
		QRcode_free(qrcode);
		qrcode = nullptr;
	}
	QByteArray ba = text.toLatin1();
	qrcode = QRcode_encodeString(ba.data(), 0, (QRecLevel)ecMode, (QRencodeMode)encMode, 1);
	if (qrcode != nullptr)
		//std::cout << "null QR code!!!" << std::endl;
	generatePixMap();
}

void QRCode::paintEvent(QPaintEvent* event) 
{
	QPainter painter(this);
	if (qrcode != NULL) {
        for (size_t y = 0; y < width; ++y) {
            for (size_t x = 0; x < width; ++x) {
				if (pixmap[y * width + x]) 
					painter.drawPoint(x, y);
			}
			
		}
	}
}


QRCoder::QRCoder(QWidget * parent)
	: QMainWindow(parent),
	ui(new Ui::QRCoder)
{
	ui->setupUi(this);

	this->setWindowTitle("QR Coder");
	this->setWindowIcon(QIcon("icons/qrcoder.ico"));
	filename = "";
	QWidget* empty = new QWidget();
	empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	empty->setFixedWidth(25);
	ui->toolBar->addWidget(ui->scaleSpinBox);
	ui->toolBar->addWidget(ui->ecModeComboBox);
	ui->toolBar->addWidget(ui->encModeComboBox);
	ui->toolBar->addWidget(empty);

	ui->toolBar->addWidget(ui->textLabel);
	ui->toolBar->addWidget(ui->textEdit);
	ui->toolBar->addWidget(ui->scanDistanceLabel);
	ui->toolBar->addWidget(ui->scanDistanceSpinBox);

	for (int i = QR_EMode::Numeric; i <= QR_EMode::FNC1SMode; i++) {
		ui->encModeComboBox->addItem(QR_EM_MODESTR[i]);
	}
	ui->encModeComboBox->setCurrentIndex(QR_EMode::DataMode);


	for (int i = QR_ECMode::Level0; i <= QR_ECMode::Level3; i++) {
		ui->ecModeComboBox->addItem(QR_EC_MODESTR[i]);
	}
	ui->ecModeComboBox->setCurrentIndex(QR_ECMode::Level0);

	ui->statusBar->setStyleSheet("QStatusBar::item { border: none; };");
	ui->distanceLabel->setFixedWidth(300);
	ui->screenDPILabel->setFixedWidth(100);
	ui->dpiLabel->setFixedWidth(200);
	ui->sizeLabel->setFixedWidth(200);
	ui->statusBar->addPermanentWidget(ui->distanceLabel);
	ui->statusBar->addPermanentWidget(ui->screenDPILabel);
	ui->statusBar->addPermanentWidget(ui->dpiLabel);
	ui->statusBar->addPermanentWidget(ui->sizeLabel);

	q = new QRCode();
	q->generateQRCode("James Dougherty");
	ui->textEdit->setText("James Dougherty");

	QWidget* widget = new QWidget(this);
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(q, 2, 2, 3, 3);
	widget->setLayout(layout);
	setCentralWidget(widget);
	ui->scaleSpinBox->setValue(q->scale);

	connect(ui->textEdit, SIGNAL(textEdited(QString)), this, SLOT(setText())); 
	connect(ui->scaleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setZoomLevel(int)));
	connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitApplication()));
	connect(ui->scanDistanceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setScanDistance(int)));
	connect(ui->encModeComboBox, SIGNAL(activated(int)), this, SLOT(encChanged(int)));
	connect(ui->ecModeComboBox, SIGNAL(activated(int)), this, SLOT(ecChanged(int)));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(actionSave()));
	connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(actionSaveAs()));
	connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(actionPrint()));


	QPalette pal = palette();
	// set black background
	pal.setColor(QPalette::Background, Qt::white);
	setAutoFillBackground(true);
	setPalette(pal);


	QScreen* screen = QGuiApplication::primaryScreen();
	QRect  screenGeometry = screen->geometry();
	
	int height = screenGeometry.height();
	int width = screenGeometry.width();
    qreal xDpi = screen->logicalDotsPerInchX();
    qreal yDpi = screen->logicalDotsPerInchY();
	screenDpi = screen->logicalDotsPerInch();

    std::cout << "screen resolution:" << width << "x" << height << std::endl;
    double size = q->width / screenDpi; /* in dpi */
	std::cout << "width (px):" << q->width << ", size:" << size << std::endl;
    ui->scanDistanceSpinBox->setValue(size * 2.54 * 10);

    qrCodeDpi = static_cast<double>(size) ; /* DPI to be scanned at the specified distance */

    updateStatusLine(static_cast<double>(size) * 2.54);

	qDebug() << "Resolution:" << screenGeometry << " X/Y DPI:" << xDpi << "/" << yDpi << " DPI:" << screenDpi;

}

void QRCoder::updateStatusLine(double size)
{
    qrCodeDpi = static_cast<double>(size) / 2.54;
	ui->distanceLabel->setText("Scannable Distance:" + QString::number(ui->scanDistanceSpinBox->value()) + " cm, " + QString::number(ui->scanDistanceSpinBox->value() / 2.54) + "in, " + QString::number(ui->scanDistanceSpinBox->value() / 2.54 / 12) + " ft");
	ui->screenDPILabel->setText("Screen DPI: " + QString::number(screenDpi) + " px");
    ui->dpiLabel->setText("Resolution: " + QString::number((qrCodeDpi * screenDpi)) + "x" + QString::number((qrCodeDpi * screenDpi)));
	ui->sizeLabel->setText("Size: " + QString::number(size) + " cm, " + QString::number(size / 2.54) + " in");
}

void QRCoder::setText()
{
	q->generateQRCode(ui->textEdit->text());
	//qDebug() << "new text:" << ui->textEdit->text();
	q->repaint();
}

void QRCoder::setZoomLevel(int level)
{
    q->setScale(static_cast<size_t>(level));
	q->generatePixMap();
	q->repaint();
	
	QScreen* screen = QGuiApplication::primaryScreen();
	screenDpi = screen->logicalDotsPerInch();
    double size = q->width / screenDpi;
    double cm = 2.54 * 10 * size;

	//update distance for new zoom level
	bool oldState = ui->scanDistanceSpinBox->blockSignals(true);
	ui->scanDistanceSpinBox->setValue((int)std::ceil(cm));
	ui->scanDistanceSpinBox->blockSignals(oldState);
	
	updateStatusLine(size * 2.54);
}

void QRCoder::setScanDistance(int distance)
{
	/* 
	 * Scan Distance / 10 = Minimum size
	 * https://scanova.io/blog/blog/2015/02/20/qr-code-minimum-size/ 
	 */
	int size = distance / 10;
    std::cout << "distance:" << distance << "cm, " << (distance / 2.54)<< "in" << std::endl;

    qrCodeDpi = static_cast<double>(size) / 2.54;

    size_t zoom = std::ceil(qrCodeDpi * screenDpi / q->getTrueWidth());
	//update zoom level for new distance
	bool oldState = ui->scaleSpinBox->blockSignals(true);
    ui->scaleSpinBox->setValue(static_cast<int>(zoom));
	ui->scaleSpinBox->blockSignals(oldState);
	
	q->setScale(zoom);
	q->generatePixMap();
	q->repaint();
	updateStatusLine(size );
}

void QRCoder::ecChanged(int mode)
{
	q->setEC(mode);
	q->generateQRCode(ui->textEdit->text());
	q->repaint();
	QScreen* screen = QGuiApplication::primaryScreen();
	screenDpi = screen->logicalDotsPerInch();
    double size = q->width / screenDpi;
	updateStatusLine(size * 2.54);
}

void QRCoder::encChanged(int mode)
{
	//only these two modes work
	if ( ( mode == QR_MODE_8) || (mode == QR_MODE_KANJI)) { 
		q->setEncoding(mode);
		q->generateQRCode(ui->textEdit->text());
		q->repaint();
	}
	QScreen* screen = QGuiApplication::primaryScreen();
	screenDpi = screen->logicalDotsPerInch();
    double size = q->width / screenDpi;
	updateStatusLine(size * 2.54);
}

QRCoder::~QRCoder()
{}

void QRCoder::exitApplication(void)
{
	QCoreApplication::exit();
}

void QRCoder::actionSaveAs(void)
{
	filename = "";
	actionSave();
}

void QRCoder::actionSave(void)
{
	QString saveExtension = "PNG";
	if (filename.isEmpty()) {
		filename = QFileDialog::getSaveFileName(this, tr("Save QR Code"), "/home/", tr("PNG(*.png);; TIFF(*.tiff *.tif);; JPEG(*.jpg *.jpeg)"));
		if (filename.isEmpty())
			return;

		int pos = filename.lastIndexOf('.');
		if (pos >= 0)
			saveExtension = filename.mid(pos + 1);
	}

	std::cout << "Save as:" << filename.toStdString() << std::endl;

    if (!q->grab(QRect(0, 0, static_cast<int>(q->width), static_cast<int>(q->width))).save(filename, qPrintable(saveExtension)))
	{
		QMessageBox::warning(this, "File could not be saved", "ok", QMessageBox::Ok);
	}
}

void QRCoder::actionPrint(void)
{
	QPrinter printer;
	
	QPrintDialog dialog(&printer, this);
	dialog.setWindowTitle(tr("Print QR Code"));
	dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
	if (dialog.exec() != QDialog::Accepted) {
		return;
	}
	else {
		printer.setFullPage(true);
		//qDebug() << "original margins:" << printer.pageLayout().margins();
		//printer.setPageMargins(QMarginsF(0, 0, 0, 0));
		//qDebug() << "new margins:" << printer.pageLayout().margins() << printer.pageRect().size();

        QPixmap px(q->grab(QRect(0, 0, static_cast<int>(q->width), static_cast<int>(q->width))));
		std::cout << px.width() << " x" << px.height() << std::endl;

		QPainter painter;
		painter.begin(&printer);

		int xpages = px.width() / printer.pageRect().size().width() + (px.width() % printer.pageRect().size().width() > 0 ? 1: 0);
		int ypages = px.height() / printer.pageRect().size().height() + (px.height() % printer.pageRect().size().height() > 0 ? 1 : 0);;
		std::cout << "xpages=" << xpages << " ypages=" << ypages << std::endl;
		std::cout << "pageres:" << printer.pageRect().size().width() << "x" << printer.pageRect().size().height() << std::endl;


		if ((px.width() > printer.pageRect().size().width()) || (px.height() > printer.pageRect().size().height())) {
			std::cout << "image is greater than page size" << std::endl;

			int xw = px.width();
			int yw = px.height();

			int xoff = 0;
			int yoff = 0;

			int xlen = 0;
			int ylen = 0;

			for (int j = 0; j < ypages; j++) {

				if (yw > printer.pageRect().size().height())
					ylen = printer.pageRect().size().height();
				else
					ylen = yw;

				for (int i = 0; i < xpages; i++) {

					if (xw > printer.pageRect().size().width())
						xlen = printer.pageRect().size().width();
					else
						xlen = xw;

					std::cout << "[" << i << "," << j << "]=<" << xoff << "," << yoff << ":" << xlen << "," << ylen << ">" << std::endl;

					QRect source(QPoint(xoff, yoff), QSize(xlen, ylen));
					QRect dest(QPoint(0, 0), QSize(xlen, ylen));
					qDebug() << "rect" << printer.pageRect();
					qDebug() << "dest" << dest;
					//painter.drawPixmap(printer.pageRect(), px, source);
					painter.drawPixmap(dest, px, source);
					printer.newPage();

					xw -= xlen;
					xoff += xlen;

				}

				xoff = xlen = 0;
				xw = px.width();

				yoff += ylen;
				yw -= ylen;

			}
		}
		else {
			std::cout << "QR:" << q->width << "x" << q->width << " fits on one page" << std::endl;
			QRect source(QPoint(0, 0), printer.pageRect().size());
			std::cout << "Printer output:" << printer.pageRect().size().width() << "x" << printer.pageRect().size().height() << std::endl;
			painter.drawPixmap(printer.pageRect(), px, source);
		}
		painter.end();


		std::cout << "Print complete" << std::endl;
	}

}
