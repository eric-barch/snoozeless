import 'package:flutter/material.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:sign_in_with_apple/sign_in_with_apple.dart';
import 'package:snoozeless/services/auth.dart';

class LoginScreen extends StatelessWidget {
  const LoginScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        padding: const EdgeInsets.all(30),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
          children: [
            const FlutterLogo(
              size: 150,
            ),
            Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                // Google login
                LoginButton(
                  icon: FontAwesomeIcons.google,
                  text: 'Sign in with Google',
                  method: AuthService().googleLogin,
                  color: Colors.blue,
                ),
                // Apple login
                FutureBuilder<Object>(
                  future: SignInWithApple.isAvailable(),
                  builder: (context, snapshot) {
                    if (snapshot.data == true) {
                      return Container(
                        margin: const EdgeInsets.only(bottom: 15),
                        child: ElevatedButton.icon(
                          icon: const Icon(
                            FontAwesomeIcons.apple,
                            color: Colors.white,
                            size: 25,
                          ),
                          style: TextButton.styleFrom(
                            padding: const EdgeInsets.all(20),
                            backgroundColor: Colors.black,
                          ),
                          onPressed: () => AuthService().appleLogin(),
                          label: const Text('Sign in with Apple'),
                        ),
                      );
                    } else {
                      return Container();
                    }
                  },
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}

class LoginButton extends StatelessWidget {
  final Color color;
  final IconData icon;
  final String text;
  final Function method;

  const LoginButton(
      {Key? key,
      required this.text,
      required this.icon,
      required this.color,
      required this.method})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Container(
      margin: const EdgeInsets.only(bottom: 15),
      child: ElevatedButton.icon(
        icon: Icon(
          icon,
          color: Colors.white,
          size: 20,
        ),
        style: TextButton.styleFrom(
          padding: const EdgeInsets.all(20),
          backgroundColor: color,
        ),
        onPressed: () => method(),
        label: Text(text),
      ),
    );
  }
}
