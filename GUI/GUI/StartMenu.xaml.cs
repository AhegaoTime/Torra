using System.Windows;
using System.Windows.Controls;

namespace GUI
{
    /// <summary>
    /// Interaction logic for StartMenu.xaml
    /// </summary>
    public partial class StartMenu : Page
    {
        //private Communicator? _c = null;
        public StartMenu(/*Communicator c*/)
        {
            //_c = c;
            InitializeComponent();
        }

        private void login_Event(object sender, RoutedEventArgs e)
        {
            //Navigation.Content = new LoginPage();
            StartMenu_Grid.Children.Remove(Login_button);
            StartMenu_Grid.Children.Remove(Singup_button);
            StartMenu_Grid.Children.Remove(Exit_button);
            StartMenu_Grid.Children.Remove(Logo_TB);
        }

        private void singup_Event(object sender, RoutedEventArgs e)
        {
            //Navigation.Content = new SingupPage(); //signup window
            StartMenu_Grid.Children.Remove(Login_button);
            StartMenu_Grid.Children.Remove(Singup_button);
            StartMenu_Grid.Children.Remove(Exit_button);
            StartMenu_Grid.Children.Remove(Logo_TB);
        }

        private void exit_Event(object sender, RoutedEventArgs e)
        {
            //this._c.CloseSocket();
            App.Current.Shutdown();
        }
    }
}
