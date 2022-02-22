import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:snoozeless/routes.dart';
import 'package:snoozeless/theme.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const App());
}

class App extends StatefulWidget {
  const App({Key? key}) : super(key: key);

  @override
  _AppState createState() => _AppState();
}

class _AppState extends State<App> {
  final Future<FirebaseApp> _initialization = Firebase.initializeApp();

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: _initialization,
      builder: (context, snapshot) {
        if (snapshot.hasError) {
          return const Text(
            'Error in main.dart',
            textDirection: TextDirection.ltr,
          );
        }
        if (snapshot.connectionState == ConnectionState.done) {
          return MaterialApp(
            debugShowCheckedModeBanner: true,
            routes: appRoutes,
            theme: appTheme,
          );
        }
        return const Text(
          'Loading',
          textDirection: TextDirection.ltr,
        );
      },
    );
  }
}
