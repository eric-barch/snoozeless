import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

var appTheme = ThemeData(
  fontFamily: GoogleFonts.workSans().fontFamily,
  brightness: Brightness.dark,
  textTheme: const TextTheme(
    bodyText1: TextStyle(
      fontSize: 18,
    ),
    bodyText2: TextStyle(
      fontSize: 16,
    ),
  ),
);
