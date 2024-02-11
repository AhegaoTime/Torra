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
    public partial class ChatMenuPage : Page
    {
        private Communicator? _c = null;

        private String _password = null;
        public ChatMenuPage(Communicator c, String password)
        {
            this._c = c;
            this._password = password;
            InitializeComponent();
        }

        private void createChat_Event(object sender, RoutedEventArgs e)
        {
            CreateChatRequest request = new CreateChatRequest();
            request.Username = Username_box.Text;
            request.Password = this._password;
            this._c.SendRequest(request);
            CreateChatResponse? response = this._c.ReceiveResponse<CreateChatResponse>();
            if (response != null)
            {
                if (response.Status)
                {
                    this.Error_TB.Visibility = Visibility.Visible;
                }
                else
                {
                    Navigation.Content = new StartMenu(this._c);
                    ChatMenuPage_Grid.Children.Remove(Logo_TB);
                    ChatMenuPage_Grid.Children.Remove(Username_TB);
                    ChatMenuPage_Grid.Children.Remove(Error_TB);
                    ChatMenuPage_Grid.Children.Remove(Username_box);
                    ChatMenuPage_Grid.Children.Remove(Return_button);
                }
            }
        }
        private void exit_Event(object sender, RoutedEventArgs e)
        {
            SignoutRequest request = new SignoutRequest();
            request.Username = Username_box.Text; 
            request.Password = this._password;
            this._c.SendRequest(request);
            SignoutResponse? response = this._c.ReceiveResponse<SignoutResponse>();
            if (response != null)
            {
                if (response.Status)
                {
                    this.Error_TB.Visibility = Visibility.Visible;
                }
                else
                {
                    Navigation.Content = new StartMenu(this._c);
                    ChatMenuPage_Grid.Children.Remove(Logo_TB);
                    ChatMenuPage_Grid.Children.Remove(Username_TB);
                    ChatMenuPage_Grid.Children.Remove(Error_TB);
                    ChatMenuPage_Grid.Children.Remove(Username_box);
                    ChatMenuPage_Grid.Children.Remove(Return_button);
                }
            }
        }

    }
}
