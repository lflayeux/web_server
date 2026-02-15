<?php
// On définit des variables en PHP
$nom_utilisateur = "Correcteur";
$heure = date("H"); // Récupère l'heure actuelle du serveur

// Une petite condition pour adapter le message
if ($heure < 17) {
    $salutation = "Bonjour";
} else {
    $salutation = "Bonsoir";
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <title>Ma page PHP</title>
</head>
<body>

    <h1><?php echo $salutation . ", " . $nom_utilisateur; ?> !</h1>
    
    <p>Aujourd'hui, nous sommes le <?php echo date("d/m/Y"); ?>.</p>

</body>
</html>