using System.Windows;
using System.Windows.Controls;

namespace Client
{
    /// <summary>
    /// Interaction logic for StartMenu.xaml
    /// </summary>
    public partial class StartMenu : Page
    {
        private Communicator? _c = null;
        private bool? _b = false;
        public StartMenu(Communicator c)
        {
            _c = c;
            InitializeComponent();
        }

        private void login_Event(object sender, RoutedEventArgs e)
        {
            if (_b == false)
            {
                GuiRequest request = new GuiRequest();
                this._c.SendRequest(request);
                _b = true;
            }
            Navigation.Content = new LoginPage(this._c);
            StartMenu_Grid.Children.Remove(Login_button);
            StartMenu_Grid.Children.Remove(Singup_button);
            StartMenu_Grid.Children.Remove(Exit_button);
            StartMenu_Grid.Children.Remove(Logo_TB);
            
        }

        private void singup_Event(object sender, RoutedEventArgs e)
        {
            if (_b == false)
            {
                GuiRequest request = new GuiRequest();
                this._c.SendRequest(request);
                _b = true;
            }
            Navigation.Content = new SingupPage(this._c); //signup window
            StartMenu_Grid.Children.Remove(Login_button);
            StartMenu_Grid.Children.Remove(Singup_button);
            StartMenu_Grid.Children.Remove(Exit_button);
            StartMenu_Grid.Children.Remove(Logo_TB);
        }

        private void exit_Event(object sender, RoutedEventArgs e)
        {
            this._c.CloseSocket();
            App.Current.Shutdown();
        }
    }
}
