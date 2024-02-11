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
    /// <summary>
    /// Interaction logic for ChatPage.xaml
    /// </summary>
    public partial class ChatPage : Page
    {
        private Communicator? _c = null;

        private String _password = null;

        private String _username = null;
        public ChatPage(Communicator c, String password, String username)
        {
            this._c = c;
            this._password = password;
            this._username = username;
            InitializeComponent();
        }

        private void SendMsgChat_Event(object sender, RoutedEventArgs e)
        {

        }

        private void exit_Event(object sender, RoutedEventArgs e)
        {
            CloseChatRequest request = new CloseChatRequest();
            request.Username = Username_box.Text;
            request.Password = this._password;
            this._c.SendRequest(request);
            CloseChatRequest? response = this._c.ReceiveResponse<CloseChatRequest>();

        }
    }
}
