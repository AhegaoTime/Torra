using System.Windows;

namespace Client
{
    public partial class MainWindow : Window
    {

        public MainWindow(Communicator c)
        {
            InitializeComponent();
            Navigation.Content = new StartMenu(c); // main menu
        }
    }
}
