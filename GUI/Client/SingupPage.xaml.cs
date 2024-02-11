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
    public partial class SingupPage : Page
    {
        private Communicator? _c = null;
        public SingupPage(Communicator c)
        {
            this._c = c;
            InitializeComponent();
        }

        private void signup_Event(object sender, RoutedEventArgs e)
        {
            SignupRequest request = new SignupRequest();
            request.Username = Username_box.Text;
            request.Password = Password_box.Password;
            this._c.SendRequest(request);
            SignupResponse? response = this._c.ReceiveResponse<SignupResponse>();
            if (response != null)
            {
                if (response.Status)
                {
                    this.Error_TB.Visibility = Visibility.Visible;
                }
                else
                {
                    Navigation.Content = new ChatMenuPage(this._c, Password_box.Password);
                    SingupPage_Grid.Children.Remove(Logo_TB);
                    SingupPage_Grid.Children.Remove(Gmail_TB);
                    SingupPage_Grid.Children.Remove(Username_TB);
                    SingupPage_Grid.Children.Remove(Password_TB);
                    SingupPage_Grid.Children.Remove(Error_TB);
                    SingupPage_Grid.Children.Remove(Gmail_box);
                    SingupPage_Grid.Children.Remove(Username_box);
                    SingupPage_Grid.Children.Remove(Password_box);
                    SingupPage_Grid.Children.Remove(Register_button);
                    SingupPage_Grid.Children.Remove(Return_button);
                }
            }
        }
        private void exit_Event(object sender, RoutedEventArgs e)
        {
            Navigation.Content = new StartMenu(this._c);
            SingupPage_Grid.Children.Remove(Logo_TB);
            SingupPage_Grid.Children.Remove(Gmail_TB);
            SingupPage_Grid.Children.Remove(Username_TB);
            SingupPage_Grid.Children.Remove(Password_TB);
            SingupPage_Grid.Children.Remove(Error_TB);
            SingupPage_Grid.Children.Remove(Gmail_box);
            SingupPage_Grid.Children.Remove(Username_box);
            SingupPage_Grid.Children.Remove(Password_box);
            SingupPage_Grid.Children.Remove(Register_button);
            SingupPage_Grid.Children.Remove(Return_button);
        }
    }
}
