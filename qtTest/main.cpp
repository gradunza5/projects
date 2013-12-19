#include <QApplication>
#include <QWidget>
#include <QPushButton>

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);

	QWidget window;

	window.resize(250, 150);
	window.setWindowTitle("Simple example");
	window.show();

	QButton button;
	button.setText("Test Button");
	button.setTooltip("Test Tooltip");
	button.show();

	return app.exec();
}
