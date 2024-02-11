using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Client
{
    public partial class LoginPage : Page
    {
        private Communicator? _c = null;
        public LoginPage(Communicator c)
        {
            _c = c;
            InitializeComponent();
        }

        private void login_Event(object sender, RoutedEventArgs e)
        {
            LoginRequest request = new LoginRequest();
            request.Username = Username_box.Text;
            request.Password = Password_box.Password;
            this._c.SendRequest(request);
            LoginResponse? response = this._c.ReceiveResponse<LoginResponse>();
            if (response != null)
            {
                if (response.Status)
                {
                    this.Error_TB.Visibility = Visibility.Visible;
                }
                else
                {
                    Navigation.Content = new ChatMenuPage(this._c, Password_box.Password);
                    LoginPage_Grid.Children.Remove(Logo_TB);
                    LoginPage_Grid.Children.Remove(Username_TB);
                    LoginPage_Grid.Children.Remove(Password_TB);
                    LoginPage_Grid.Children.Remove(Error_TB);
                    LoginPage_Grid.Children.Remove(Username_box);
                    LoginPage_Grid.Children.Remove(Password_box);
                    LoginPage_Grid.Children.Remove(Login_button);
                    LoginPage_Grid.Children.Remove(Return_button);
                }
            }
        }

        private void exit_Event(object sender, RoutedEventArgs e)
        {
            Navigation.Content = new StartMenu(this._c);
            LoginPage_Grid.Children.Remove(Logo_TB);
            LoginPage_Grid.Children.Remove(Username_TB);
            LoginPage_Grid.Children.Remove(Password_TB);
            LoginPage_Grid.Children.Remove(Error_TB);
            LoginPage_Grid.Children.Remove(Username_box);
            LoginPage_Grid.Children.Remove(Password_box);
            LoginPage_Grid.Children.Remove(Login_button);
            LoginPage_Grid.Children.Remove(Return_button);
        }
    }
}
